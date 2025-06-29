#include "Common.hlsli"

cbuffer CubeMappingVertexConstantData : register(b0)
{
    matrix viewProj;
};

struct CubeMappingPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

struct CubeMappingVertexShaderInput
{
    float3 posModel : POSITION;
};

CubeMappingPixelShaderInput main(CubeMappingVertexShaderInput input)
{
    CubeMappingPixelShaderInput output;
    output.posModel = input.posModel;
    output.posProj = mul(float4(input.posModel, 1.0f), viewProj);
    return output;
}