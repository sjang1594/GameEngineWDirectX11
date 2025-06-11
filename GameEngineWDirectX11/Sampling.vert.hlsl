#include "Common.hlsli"

struct SamplingVertexShaderInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
    SamplingPixelShaderInput output;
    output.position = float4(input.position, 1.0f);
    output.texcoord = input.texCoord;
    return output;
}