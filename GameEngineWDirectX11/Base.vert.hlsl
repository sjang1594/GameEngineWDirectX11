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
    output.color = float3(0.0f, 0.0f, 0.0f);
    
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose);
    output.normalWorld = normalize(output.normalWorld);
    return output;
}