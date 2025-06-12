TextureCube g_textureCube0 : register(t0);
SamplerState g_sampler : register(s0);

struct CubeMappingPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

float4 main(CubeMappingPixelShaderInput input) : SV_Target0
{
    return g_textureCube0.Sample(g_sampler, input.posModel.xyz);
}