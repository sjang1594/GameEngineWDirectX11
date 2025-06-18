#include "Common.hlsli"

cbuffer BasicVertexConstantData : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

PixelShaderInput main(VertexShaderInput input) {
    PixelShaderInput output;

    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, model);
    output.posWorld = pos.xyz; // model in world
    
    pos = mul(pos, view);
    pos = mul(pos, projection);
    
    output.posProj = pos;
    output.texcoord = input.texcoord;

    // Normal
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalW = mul(normal, invTranspose);
    output.normalW = normalize(output.normalW);
    
    // Tangent
    float4 tangent = float4(input.tangentWorld, 0.0f); // tangent in world
    tangent = mul(tangent, model);
    output.tangentW = tangent.xyz;
    
    output.color = float3(0.0f, 0.0f, 0.0f);
    return output;
}