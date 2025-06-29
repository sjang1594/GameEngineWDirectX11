#include "Common.hlsli"

// Vertex Shader에서도 텍스춰 사용
Texture2D g_heightTexture : register(t0);

cbuffer MeshConstants : register(b0)
{
    matrix world; 
    matrix worldIT; // World의 InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

PixelShaderInput main(VertexShaderInput input) {
    PixelShaderInput output;
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, worldIT).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    float4 tangentWorld = float4(input.tangentModel, 0.0f);
    tangentWorld = mul(tangentWorld, world);

    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, world);
    
    if (useHeightMap)
    {
        // VertexShader에서는 SampleLevel 사용
        float height = g_heightTexture.SampleLevel(linearClampSampler, input.texcoord, 0).r;
        height = height * 2.0 - 1.0;
        pos += float4(output.normalWorld * height * heightScale, 0.0);
    }
    
    
    output.posWorld = pos.xyz;

    pos = mul(pos, viewProj);

    output.posProj = pos;
    output.texcoord = input.texcoord;
    output.tangentWorld = tangentWorld.xyz;
    
    return output;
}