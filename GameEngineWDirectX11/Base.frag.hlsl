#include "Common.hlsli"

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D heightTexture : register(t2);
Texture2D aoTexture : register(t3);
Texture2D metallicRoughnessTexture : register(t4);
Texture2D emissiveTexture : register(t5);

cbuffer MaterialConstants : register(b0)
{
    float exposure;
    float gamma;
    int reverseNormalY;
    int isParallax;
};

// fresnelR0
// Water : (0.02, 0.02, 0.02)
// Glass : (0.08, 0.08, 0.08)
// Plastic : (0.05, 0.05, 0.05)
// Gold: (1.0, 0.71, 0.29)
// Silver: (0.95, 0.93, 0.88)
// Copper: (0.95, 0.64, 0.54)
static const float3 Fdielectric = 0.04;

float3 SchlickFresnelUnrealOld(float3 F0, float NdotH)
{
    return F0 + (1 - F0) * pow(2, (-5.5473 * NdotH - 5.98316) * NdotH);
}

// These are used for current Unreal Engine
float3 SchlickFresnel(float3 F0, float3 normal, float3 toEye)
{
    float cosTheta = saturate(dot(normal, toEye));
    return F0 + (1.0f - F0) * pow(1 - cosTheta, 5.0f);
}

float3 SpecularIBL(float3 albedo, float3 normalW, float3 pixelToEye, float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.Sample(linearClampSampler, float2(dot(normalW, pixelToEye), 1.0 - roughness)).rg;
    float3 specularIrradiance = specularIBLTex.SampleLevel(linearWrapSampler, reflect(-pixelToEye, normalW), roughness * 5.0f).rgb;
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    return (F0 * specularBRDF.r + specularBRDF.g) * specularIrradiance;
}

float3 DiffuseIBL(float3 albedo, float3 normalW, float3 pixelToEye, float3 metallic, float roughness)
{
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnelUnrealOld(F0, max(0.0, dot(normalW, pixelToEye)));
    float3 kd = lerp(1.0 - F, 0.0, metallic);
    float3 irradiance = irradianceIBLTex.Sample(linearWrapSampler, normalW).rgb;
    return kd * albedo * irradiance;
}

float3 AmbientLightingIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao, float metallic, float roughness)
{
    float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic, roughness);
    return (specularIBL + diffuseIBL) * ao;
}

// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// D(X) NDF Function
// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float NdfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

float SchlickG1(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

// G(x) => Specular G
float SchlickGGX(float NdotI, float NdotO, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return SchlickG1(NdotI, k) * SchlickG1(NdotO, k);
}

uint querySpecularTextureLevels()
{
    uint width, height, levels;
    specularIBLTex.GetDimensions(0, width, height, levels);
    return levels;
}

// Material Params
float3 SampleAlbedo(float2 uv, float lod)
{
    return albedoTexture.SampleLevel(linearWrapSampler, uv, lod);
}

float3 SampleNormal(float2 uv, float lod, bool reverseY)
{
    float3 normalTex = normalTexture.SampleLevel(linearWrapSampler, uv, lod).rgb * 2.0 - 1.0;
    if (reverseY)
        normalTex.y = -normalTex.y;
    return normalTex;
}

float SampleAO(float2 uv, float lod)
{
    return aoTexture.SampleLevel(linearWrapSampler, uv, lod).r;
}

float SampleRoughness(float2 uv, float lod)
{
    return metallicRoughnessTexture.SampleLevel(linearWrapSampler, uv, lod).g;
}

float SampleMetallic(float2 uv, float lod)
{
    return metallicRoughnessTexture.SampleLevel(linearWrapSampler, uv, lod).b;
}

float3 SampleEmissive(float2 uv, float lod)
{
    return emissiveTexture.SampleLevel(linearWrapSampler, uv, lod).rgb;
}

void RetrieveMaterialParam(float2 uv, float lod, bool reverseY, 
                           out float3 albedo, out float3 normalWorld, out float ao, 
                           out float roughness, out float metallic, out float3 emissive, 
                           float3x3 TBN)
{
    albedo = SampleAlbedo(uv, lod);
    float3 normalTex = SampleNormal(uv, lod, reverseY);
    ao = SampleAO(uv, lod);
    roughness = SampleRoughness(uv, lod);
    metallic = SampleMetallic(uv, lod);
    emissive = SampleEmissive(uv, lod);
    normalWorld = normalize(mul(normalTex, TBN));
}

// https://www.d3dcoder.net/Data/Resources/ParallaxOcclusion.pdf
float2 ParallaxOcclusionMap(float2 inputTexture, float3 normalW, float3 viewDir, float3 viewDirTS, float distance)
{
    float2 maxParallaxOffset = -viewDirTS.xy * 0.002 / max(viewDirTS.z, 0.01);
   
    float angleFactor = dot(viewDir, normalW);
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
        currHeight = heightTexture.SampleGrad(linearWrapSampler, inputTexture + currTexOffset, dx, dy).r;
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

float3 CalculateSpecularBRDF(float3 F, float NdotI, float NdotH, float NdotO, float roughness)
{
    float D = NdfGGX(NdotH, roughness);
    float G = SchlickGGX(NdotI, NdotO, roughness);
    return (F * D * G) / max(EPSILON, 4.0 * NdotI * NdotO);
    // return (F * D * G) / max(EPSILON, PI * NdotI * NdotO);
}

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(PixelShaderInput input)
{
    float2 uv = input.texcoord;
    float3 toEye = normalize(eyeWorld - input.posWorld);
    float3 viewDir = -toEye;
    float dist = length(eyeWorld - input.posWorld);
    float lod = 0.0;
    
    float3 N = input.normalW;
    float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    
    float3 viewDirTS = mul(viewDir, TBN);
    viewDirTS = normalize(viewDirTS);
    
    //if (isParallax)
    //{
    //    uv = ParallaxOcclusionMap(uv, input.normalW, viewDir, viewDirTS, dist);
    //}
    
    float3 albedo, normalWorld, emissive;
    float ao, roughness, metallic;
    
    RetrieveMaterialParam(uv, lod, reverseNormalY, albedo, normalWorld,
                          ao, roughness, metallic, emissive, TBN);
    
    float3 ambientLighting = AmbientLightingIBL(albedo, normalWorld, toEye, ao, metallic, roughness);
    float3 directLighting = float3(0, 0, 0);
    
    int i = 0;
    //[unroll]
    //for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    //{
    //     color += ComputeDirectionalLight(light[i], material, normalWorld,
    //                                     toEye);
    // }
    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        float3 lightVec = light[i].position - input.posWorld;
        float3 halfway = normalize(toEye * lightVec);
        float NdotI = max(0.0, dot(normalWorld, lightVec));
        float NdotH = max(0.0, dot(normalWorld, halfway));
        float NdotO = max(0.0, dot(normalWorld, toEye));
        
        float3 F0 = lerp(Fdielectric, albedo, metallic);
        float3 F = SchlickFresnelUnrealOld(F0, max(0.0f, dot(halfway, toEye)));
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
        float3 diffuseBRDF = kd * albedo;
        
        float3 specularBRDF = CalculateSpecularBRDF(F, NdotI, NdotH, NdotO, roughness);
        float3 radiance = light[i].radiance * saturate((light[i].fallOffEnd - length(lightVec)) / (light[i].fallOffEnd - light[i].fallOffStart));
        directLighting += (diffuseBRDF + specularBRDF) * radiance * NdotI;
    }
    // [unroll]
    // for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    // {
    //    color += ComputeSpotLight(light[i], material, input.posWorld, normalWorld, toEye);
    //}
    
    PixelShaderOutput output;
    output.pixelColor = float4(ambientLighting + directLighting + emissive, 1.0);
    //output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
    return output;
}
