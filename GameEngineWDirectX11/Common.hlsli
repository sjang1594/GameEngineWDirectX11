#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define MAX_LIGHTS 3
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1
#define NUM_SPOT_LIGHTS 1

struct Material
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float dummy1; // 16
    float3 specular;
    float dummy2;
    float3 fresnelR0;
    float dummy3;
};

struct Light
{
    float3 strength;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
};

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal,
                   float3 toEye, Material mat)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    float3 specular = mat.specular * pow(max(hdotn, 0.0f), mat.shininess);

    return mat.ambient + (mat.diffuse + specular) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
    float3 lightVec = -L.direction;
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.strength * ndotl;
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float3 CalcAttenuation(float d, float fallOffStart, float fallOffEnd)
{
    return saturate((fallOffEnd - d) / (fallOffEnd - fallOffStart));
}

float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.position - pos;
    float distance = length(lightVec);
    if (distance > L.fallOffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        lightVec /= distance;
        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.strength * ndotl;
        float attenuationFactor = CalcAttenuation(distance, L.fallOffStart, L.fallOffEnd);
        lightStrength *= attenuationFactor;
        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
}

float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.position - pos;
    float distance = length(lightVec);
    
    if (distance > L.fallOffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        lightVec /= distance;
        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.strength * ndotl;
        
        float attenuationFactor = CalcAttenuation(distance, L.fallOffStart, L.fallOffEnd);
        lightStrength *= attenuationFactor;
        
        float spotFactor = pow(max(-dot(lightVec, L.direction), 0.0f), L.spotPower);
        lightStrength *= spotFactor;
        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
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
    float3 posWorld : POSITION;   // World Position
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD;
    float3 tangentWorld : TANGENT0;
    float3 color : COLOR;
};

#endif // __COMMON_HLSLI__