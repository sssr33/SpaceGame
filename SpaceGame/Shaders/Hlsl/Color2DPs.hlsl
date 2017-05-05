
struct PsInput {
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

float4 main(PsInput input) : SV_TARGET{
    float4 color = input.color;
    color *= color.a;
    return color;
}