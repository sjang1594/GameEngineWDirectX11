#include "Common.hlsli"

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.pixelColor = float4(0, 0, 0, 1);
    
    return output;
}
