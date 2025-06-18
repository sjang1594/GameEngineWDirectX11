#include "Common.hlsli"

struct GeometryShaderInput
{
    float4 posModel : SV_POSITION;
    float3 normalW : NORMAL;
};

GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;
    output.posModel = float4(input.posModel, 1.0);
    output.normalW = input.normalModel;
    return output;
}