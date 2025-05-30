#include "Common.hlsli"

PixelShaderInput main(VertexShaderInput input) {
    PixelShaderInput output = (PixelShaderInput)0;
    output.posWorld = float3(0.0f, 0.0f, 0.0f);
    return output;
}