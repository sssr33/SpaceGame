
Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct PsInput {
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PsInput input) : SV_TARGET{
    float4 color = tex.Sample(samplerState, input.tex);
    return color;
}