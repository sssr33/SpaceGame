
cbuffer Matrices : register(b0) {
    matrix mvp;
    matrix toPixels;
    matrix toProjected;
    float4 params;
};

struct VsInput {
    float2 pos : POSITION0;
    float2 adjPrev : TEXCOORD0;
    float2 adjNext : TEXCOORD1;
    float aaDir : TEXCOORD2;
    float4 color : COLOR0;
};

struct PsInput {
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

float2 rot90(float2 v) {
    float2 rotated;

    rotated.x = -v.y;
    rotated.y = v.x;

    return rotated;
}

float2 calcNorm2d(float2 a, float2 b) {
    float2 vec = a - b;
    float len = length(vec);

    if (len == 0.0f) {
        vec = float2(0.0f, 0.0f);
    }
    else {
        vec /= len;

    }

    return vec;
}

PsInput main(VsInput input) {
    PsInput output = (PsInput)0;

    output.color = input.color;

    if (input.aaDir != 0) {
        float4 posTmp = float4(input.pos, 0.0f, 1.0f);
        float4 posPrevTmp = float4(input.adjPrev, 0.0f, 1.0f);
        float4 posNextTmp = float4(input.adjNext, 0.0f, 1.0f);

        posTmp = mul(posTmp, mvp);
        posPrevTmp = mul(posPrevTmp, mvp);
        posNextTmp = mul(posNextTmp, mvp);

        posTmp /= posTmp.w;
        posPrevTmp /= posPrevTmp.w;
        posNextTmp /= posNextTmp.w;

        posTmp.w = 1.0f;
        posPrevTmp.w = 1.0f;
        posNextTmp.w = 1.0f;

        float2 posTmp2d = mul(posTmp, toPixels).xy;
        float2 posPrevTmp2d = mul(posPrevTmp, toPixels).xy;
        float2 posNextTmp2d = mul(posNextTmp, toPixels).xy;

        /*float2 vecPrev2d = normalize(posPrevTmp2d - posTmp2d);
        float2 vecNext2d = normalize(posTmp2d - posNextTmp2d);*/

        float2 vecPrev2d = calcNorm2d(posPrevTmp2d, posTmp2d);
        float2 vecNext2d = calcNorm2d(posTmp2d, posNextTmp2d);

        vecPrev2d = rot90(vecPrev2d);
        vecNext2d = rot90(vecNext2d);

        // TODO add better vector calculation, taking into accound needed thickness of AA

        float2 aaVec = normalize(vecPrev2d + vecNext2d) * 1.0f * input.aaDir;

        posTmp2d += aaVec;

        posTmp.xy = mul(float4(posTmp2d, 0.0f, 1.0f), toProjected).xy;

        output.pos = posTmp;
        output.color.a = 0;

        /*float4 posTmp = float4(input.pos, 0.0f, 1.0f);
        float4 posAATmp = float4(input.pos + input.aaVec, 0.0f, 1.0f);

        posTmp = mul(posTmp, mvp);
        posAATmp = mul(posAATmp, mvp);

        posTmp /= posTmp.w;
        posAATmp /= posAATmp.w;

        posTmp.w = 1.0f;
        posAATmp.w = 1.0f;

        float4 vec3 = posAATmp - posTmp;

        if (posAATmp.z < 0.0f) {
            float t = posTmp.z - 0.0f / posTmp.z - posAATmp.z;
            posAATmp = posTmp + vec3 * t;
        }

        float2 posTmp2d = mul(posTmp, toPixels).xy;
        float2 posAATmp2d = mul(posAATmp, toPixels).xy;

        float2 aaVec = normalize(posAATmp2d - posTmp2d) * 1.5f;

        posAATmp2d = posTmp2d + aaVec;

        posTmp.xy = mul(float4(posAATmp2d, 0.0f, 1.0f), toProjected).xy;

        output.pos = posTmp;
        output.color.a = 0;*/


        /*aaTmp.xy += params.xy;

        aaTmp = mul(aaTmp, toProjected);
        output.pos = mul(posTmp, mvp);

        aaTmp.zw = 0.0f;
        output.color.a = 0;
        output.pos /= output.pos.w;
        output.pos.w = 1.0f;

        output.pos += aaTmp;*/

        /*float4 aaTmp = float4(input.aaVec, 0.0f, 1.0f);
        float4 posTmp = float4(input.pos, 0.0f, 1.0f);

        aaTmp.xy += params.xy;

        aaTmp = mul(aaTmp, toProjected);
        output.pos = mul(posTmp, mvp);

        aaTmp.zw = 0.0f;
        output.color.a = 0;
        output.pos /= output.pos.w;
        output.pos.w = 1.0f;

        output.pos += aaTmp;*/

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