#include "Common.hlsli"

// TODO: Switch to BRDF
TextureCube g_diffuseCube : register(t0);
TextureCube g_specularCube : register(t1);

Texture2D g_albedoTexture : register(t2);
Texture2D g_normalTexture : register(t3);
Texture2D g_heightTexture : register(t4);
Texture2D g_aoTexture : register(t5);
Texture2D g_roughnessTexture : register(t6);

SamplerState g_sampler : register(s0);

cbuffer BasicPixelConstantData : register(b0)
{
    float3 eyeWorld;
    float dummy1;
    Material material;
    Light light[MAX_LIGHTS];
    int reverseNormalMapY; // In case the normal map is flipped vertically
    int heightScale;
    float2 dummy2;
}

// fresnelR0
// Water : (0.02, 0.02, 0.02)
// Glass : (0.08, 0.08, 0.08)
// Plastic : (0.05, 0.05, 0.05)
// Gold: (1.0, 0.71, 0.29)
// Silver: (0.95, 0.93, 0.88)
// Copper: (0.95, 0.64, 0.54)
float3 SchlickFresnel(float3 fresnelR0, float3 normal, float3 toEye)
{
    float cosTheta = saturate(dot(normal, toEye));
    return fresnelR0 + (1.0f - fresnelR0) * pow(1 - cosTheta, 5.0f);
}

// https://www.d3dcoder.net/Data/Resources/ParallaxOcclusion.pdf
float2 ParallaxOcclusionMap(float2 inputTexture, float3 normalWorld, float3 viewDir, float3 viewDirTS, float distance)
{
    float2 maxParallaxOffset = -viewDirTS.xy * 0.002 / max(viewDirTS.z, 0.01);
   
    float angleFactor = dot(viewDir, normalWorld);
    const int minLayer = 8;
    const int maxLayer = 32;
    int sampleCount = (int) lerp(maxLayer, minLayer, angleFactor);
    
    float layerDepth = 1.0f / (float) sampleCount;
    float2 deltaUV = maxParallaxOffset * layerDepth;
    
    float2 dx = ddx(inputTexture);
    float2 dy = ddy(inputTexture);
    
    int sampleIndex = 0;
    float2 currTexOffset = 0;
    float2 prevTexOffset = 0;
    float2 finalTexOffset = 0;
    float currRayZ = 1.0f - layerDepth;
    float prevRayZ = 1.0f;
    float currHeight = 0.0f;
    float prevHeight = 0.0f;

    
    while (sampleIndex < sampleCount + 1)
    {
        
        currHeight = g_heightTexture.SampleGrad(g_sampler, inputTexture + currTexOffset, dx, dy).r;
        currHeight = smoothstep(0.2, 0.8, currHeight);
        if (currHeight > currRayZ)
        {
            // Interpolation
            float t = (prevHeight - prevRayZ) / (prevHeight - currHeight + currRayZ - prevRayZ);
            finalTexOffset = prevTexOffset + t * deltaUV;
            sampleIndex = sampleCount + 1;
        }
        else
        {
            ++sampleIndex;
            prevTexOffset = currTexOffset;
            prevRayZ = currRayZ;
            prevHeight = currHeight;
            currTexOffset += deltaUV;
            currRayZ -= layerDepth;
        }
    }
   
    return clamp(inputTexture + finalTexOffset, 0, 1);
}

float4 main(PixelShaderInput input) : SV_TARGET {
    float3 toEye = normalize(eyeWorld - input.posWorld);
    float3 viewDir = -toEye;
    float dist = length(eyeWorld - input.posWorld);
    float distMin = 3.0;
    float distMax = 10.0;
    float lod = 10.0 * saturate(dist / (distMax - distMin));
    float3 N = input.normalWorld;
    float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    float3 viewDirTS = mul(viewDir, TBN);
    viewDirTS = normalize(viewDirTS);
    
    float2 parallaxUV = ParallaxOcclusionMap(input.texcoord, input.normalWorld, viewDir, viewDirTS, dist);
    
    float2 dx = ddx(parallaxUV);
    float2 dy = ddy(parallaxUV);
    
    float4 albedo = g_albedoTexture.SampleLevel(g_sampler, parallaxUV, lod);
    float3 normalTex = g_normalTexture.SampleLevel(g_sampler, parallaxUV, lod).rgb * 2.0 - 1.0;
    if (reverseNormalMapY)
    {
        normalTex.y = -normalTex.y;
    }
    float ao = g_aoTexture.SampleLevel(g_sampler, parallaxUV, lod).r;
    float roughness = g_roughnessTexture.SampleLevel(g_sampler, parallaxUV, lod).r;
    
    float3 normalWorld = normalize(mul(normalTex, TBN));
    float3 color = float3(0, 0, 0);
    int i = 0;
    [unroll]
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        color += ComputeDirectionalLight(light[i], material, normalWorld,
                                         toEye);
    }
    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        color += ComputePointLight(light[i], material, input.posWorld, normalWorld, toEye);
    }
    [unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        color += ComputeSpotLight(light[i], material, input.posWorld, normalWorld, toEye);
    }
    
    // CubeMap & Fresnel
    float4 diffuseEnv = g_diffuseCube.Sample(g_sampler, normalWorld) + float4(color, 1.0);
    float4 specularEnv = g_specularCube.Sample(g_sampler, reflect(-toEye, normalWorld));
    diffuseEnv *= float4(material.diffuse, 1.0);
    specularEnv *=
        pow(abs(specularEnv.r + specularEnv.g + specularEnv.b) / 3.0, material.shininess);
    specularEnv *= float4(material.specular, 1.0);
    
    float3 f = SchlickFresnel(material.fresnelR0, normalWorld, toEye);
    specularEnv.rgb *= f;
    specularEnv *= pow(saturate(1.0 - roughness), 2.0);
    
    diffuseEnv *= albedo;
    diffuseEnv *= ao;

    float3 finalColor = diffuseEnv.rgb + specularEnv.rgb;
    return float4(finalColor, 1.0);
}
