#include "Common.hlsli"
cbuffer BasicVertexConstantData : register(b0)
{
    matrix world;
    matrix worldIT;
    int useHeightMap;
    float heightScale;
    float2 dummy2;
};

struct NormalGeometryShaderInput
{
    float4 posModel : SV_POSITION;
    float3 normalModel : NORMAL;
};

struct NormalPixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

static const float lineScale = 0.02;

[maxvertexcount(2)]
void main(point NormalGeometryShaderInput input[1], inout LineStream<NormalPixelShaderInput> outputStream)
{
    NormalPixelShaderInput output;
    float4 posW = mul(input[0].posModel, world);
    float4 normalModel = float4(input[0].normalModel, 0.0);
    float4 normalW = mul(normalModel, worldIT);
    
    normalW = float4(normalize(normalW.xyz), 0.0);
    output.pos = mul(posW, viewProj);
    output.color = float3(1.0, 1.0, 0.0);
    outputStream.Append(output);
   
    output.pos = mul(posW + 0.1 * lineScale * normalW, viewProj);
    output.color = float3(1.0, 0.0, 0.0);
    outputStream.Append(output);
}