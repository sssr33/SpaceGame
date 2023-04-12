
Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct PsInput {
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
    float2 tex : TEXCOORD0;
};

float4 main(PsInput input) : SV_TARGET {
    float4 texColor = tex.Sample(samplerState, input.tex);
    float4 color = input.color * texColor;
    color *= color.a;
    return color;
}