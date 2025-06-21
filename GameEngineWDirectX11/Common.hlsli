#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define MAX_LIGHTS 3
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1
#define NUM_SPOT_LIGHTS 1

static const float PI = 3.14159265f;
static const float EPSILON = 1e-6f;

// Sampler State 
SamplerState linearWrapSampler : register(s0);
SamplerState linearClampSampler : register(s1);

// HDRI & Cubemapping Texture starting from t10
TextureCube envIBLTex : register(t10);
TextureCube specularIBLTex : register(t11);
TextureCube irradianceIBLTex : register(t12);
Texture2D brdfTex : register(t13);

struct Light
{
    float3 radiance;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
};

cbuffer GlobalConstants : register(b1)
{
    matrix view;
    matrix proj;
    matrix viewProj;
    float3 eyeWorld;
    float strengthIBL;
    int textureToDraw = 0;
    float envLodBias = 0.0f;
    float dummy;
    Light light[MAX_LIGHTS];
}

struct VertexShaderInput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen Pos
    float3 posWorld : POSITION; // World Position
    float3 normalW : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentW : TANGENT0;
};

//float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal,
//                   float3 toEye, Material mat)
//{
//    float3 halfway = normalize(toEye + lightVec);
//    float hdotn = dot(halfway, normal);
//    float3 specular = mat.specular * pow(max(hdotn, 0.0f), mat.shininess);
//
//    return mat.ambient + (mat.diffuse + specular) * lightStrength;
//}
//
//float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
//{
//    float3 lightVec = -L.direction;
//    float ndotl = max(dot(lightVec, normal), 0.0f);
//    float3 lightStrength = L.radiance * ndotl;
//    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
//}
//
//float3 CalcAttenuation(float d, float fallOffStart, float fallOffEnd)
//{
//    return saturate((fallOffEnd - d) / (fallOffEnd - fallOffStart));
//}
//
//float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
//{
//    float3 lightVec = L.position - pos;
//    float distance = length(lightVec);
//    if (distance > L.fallOffEnd)
//    {
//        return float3(0.0f, 0.0f, 0.0f);
//    }
//    else
//    {
//        lightVec /= distance;
//        float ndotl = max(dot(lightVec, normal), 0.0f);
//        float3 lightStrength = L.radiance * ndotl;
//        float attenuationFactor = CalcAttenuation(distance, L.fallOffStart, L.fallOffEnd);
//        lightStrength *= attenuationFactor;
//        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
//    }
//}
//
//float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
//{
//    float3 lightVec = L.position - pos;
//    float distance = length(lightVec);
//    
//    if (distance > L.fallOffEnd)
//    {
//        return float3(0.0f, 0.0f, 0.0f);
//    }
//    else
//    {
//        lightVec /= distance;
//        float ndotl = max(dot(lightVec, normal), 0.0f);
//        float3 lightStrength = L.radiance * ndotl;
//        
//        float attenuationFactor = CalcAttenuation(distance, L.fallOffStart, L.fallOffEnd);
//        lightStrength *= attenuationFactor;
//        
//        float spotFactor = pow(max(-dot(lightVec, L.direction), 0.0f), L.spotPower);
//        lightStrength *= spotFactor;
//        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
//    }
// }

#endif // __COMMON_HLSLI__