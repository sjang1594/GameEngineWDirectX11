TextureCube g_envTex : register(t0);
TextureCube g_specularTex : register(t1);
TextureCube g_irradianceTex : register(t2);
SamplerState g_sampler : register(s0);

cbuffer CubeMappingPixelConstantData : register(b0)
{
    int textureToDraw = 0;
    float mipLevel = 0.0f;
    float dummy1;
    float dummy2;
};

struct CubeMappingPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

float4 main(CubeMappingPixelShaderInput input) : SV_Target0
{
    if (textureToDraw == 0)
    {
        return g_envTex.SampleLevel(g_sampler, input.posModel.xyz, mipLevel);
    }
    else if (textureToDraw == 1)
    {
        return g_specularTex.SampleLevel(g_sampler, input.posModel.xyz, mipLevel);
    }
    else
    {
        return g_irradianceTex.SampleLevel(g_sampler, input.posModel.xyz, mipLevel);
    }
    return float4(0.0, 0.0, 1.0, 0.0);
}