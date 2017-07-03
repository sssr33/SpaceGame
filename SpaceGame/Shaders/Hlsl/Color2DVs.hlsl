
cbuffer Matrices : register(b0) {
    matrix mvp;
    matrix toPixels;
    matrix toProjected;
    float4 params;
};

struct VsInput {
    float2 pos : POSITION0;
    float2 aaVec : TEXCOORD0;
    float4 color : COLOR0;
};

struct PsInput {
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

PsInput main(VsInput input) {
    PsInput output = (PsInput)0;

    output.color = input.color;

    if (any(input.aaVec)) {
        float4 aaTmp = float4(input.aaVec, 0.0f, 1.0f);
        float4 posTmp = float4(input.pos, 0.0f, 1.0f);

        aaTmp.xy += params.xy;

        aaTmp = mul(aaTmp, toProjected);
        output.pos = mul(posTmp, mvp);

        aaTmp.zw = 0.0f;
        output.color.a = 0;
        output.pos /= output.pos.w;
        output.pos.w = 1.0f;

        output.pos += aaTmp;

        //float4 aaTmp = float4(input.aaVec, 0.0f, 1.0f);
        //aaTmp.xy += params.xy;

        //aaTmp = mul(aaTmp, toProjected);
        //aaTmp.zw = 0.0f;
        //float4 posTmp = float4(input.pos, 0.0f, 1.0f) + aaTmp;
        //output.pos = mul(posTmp, mvp);
        ///*output.pos += float4(input.aaVec, 0.0f, 0.0f);
        //output.pos = mul(output.pos, toProjected);*/
        //output.color.a = 0;


        //float4 aaTmp = mul(float4(input.aaVec, 0.0f, 1.0f), toProjected);
        //aaTmp.zw = 0.0f;
        //float4 posTmp = float4(input.pos, 0.0f, 1.0f) + aaTmp;
        //output.pos = mul(posTmp, mvp);
        ///*output.pos += float4(input.aaVec, 0.0f, 0.0f);
        //output.pos = mul(output.pos, toProjected);*/
        //output.color.a = 0;

        /*output.pos = mul(float4(input.pos, 0.0f, 1.0f), toPixels);
        output.pos += float4(input.aaVec, 0.0f, 0.0f);
        output.pos = mul(output.pos, toProjected);
        output.color.a = 0;*/
    }
    else {
        output.pos = mul(float4(input.pos, 0.0f, 1.0f), mvp);
    }

    return output;
}