#include "Common.hlsli"

struct CubeMappingPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

CubeMappingPixelShaderInput main(VertexShaderInput input)
{
    CubeMappingPixelShaderInput output;
    output.posModel = input.posModel;
    output.posProj = mul(float4(input.posModel, 1.0f), view);
    output.posProj = mul(float4(output.posProj.xyz, 1.0f), proj);
    return output;
}