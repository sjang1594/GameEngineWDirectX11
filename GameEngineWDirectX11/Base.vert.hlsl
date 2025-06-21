#include "Common.hlsli"

cbuffer BasicVertexConstantData : register(b0)
{
    matrix world;
    matrix worldIT;
    float heightScale;
    float3 dummy2;
};

PixelShaderInput main(VertexShaderInput input) 
{
    PixelShaderInput output;

    // Normal
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalW = mul(normal, worldIT).xyz;
    output.normalW = normalize(output.normalW);
    
    // Tangent
    float4 tangent = float4(input.tangentWorld, 0.0f); // tangent in world
    tangent = mul(tangent, world);
    
    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, world);
    
    output.posWorld = pos.xyz; // model in world
    pos = mul(pos, viewProj);
    output.posProj = pos;
    output.texcoord = input.texcoord;
    output.tangentW = tangent.xyz;
    
    return output;
}