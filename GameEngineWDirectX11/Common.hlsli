#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define MAX_LIGHTS 3 
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

SamplerState linearWrapSampler : register(s0);
SamplerState linearClampSampler : register(s1);

TextureCube envIBLTex : register(t10);
TextureCube specularIBLTex : register(t11);
TextureCube irradianceIBLTex : register(t12);
Texture2D brdfTex : register(t13);

struct Light
{
    float3 radiance; // Strength
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
    
    uint type;
    float radius;
    float haloRadius;
    float haloStrength;

    matrix viewProj;
    matrix invProj;
};

struct VertexShaderInput
{
    float3 posModel : POSITION;     // Model Position in Local Coordinate
    float3 normalModel : NORMAL0;   // Model Normal 
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION;   // Screen position
    float3 posWorld : POSITION;     // World position 
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
};

cbuffer GlobalConstants : register(b1) {
    matrix view;        // 64
    matrix proj;        // 64
    matrix invProj;     // 64
    matrix viewProj;    // 64
    matrix invViewProj; // 64
    float3 eyeWorld;    // 12
    
    float strengthIBL;  // 4
    int textureToDraw = 0; // 4
    float envLodBias = 0.0f; // 4 
    float lodBias = 2.0f;
    float dummy2;
    Light lights[MAX_LIGHTS];
};

float random(float3 seed, int i)
{
    float4 seed4 = float4(seed, i);
    float dotP = dot(seed4, float4(12.9898, 78.233, 45.164, 94.673));
    return frac(sin(dotP) * 43758.5453);
}

#endif // __COMMON_HLSLI__