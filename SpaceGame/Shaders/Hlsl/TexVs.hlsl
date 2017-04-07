
cbuffer Matrices : register(b0) {
    matrix mvp;
};

struct VsInput {
    float2 pos : POSITION0;
    float2 tex : TEXCOORD0;
};

struct PsInput {
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PsInput main(VsInput input) {
    PsInput output = (PsInput)0;

    output.pos = mul(float4(input.pos, 0.0f, 1.0f), mvp);
    output.tex = input.tex;

    return output;
}