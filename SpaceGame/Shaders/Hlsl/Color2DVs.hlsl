
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

float2 rot90cw(float2 v) {
    float2 rotated;

    rotated.x = -v.y;
    rotated.y = v.x;

    return rotated;
}

float2 rot90ccw(float2 v) {
    float2 rotated;

    rotated.x = v.y;
    rotated.y = -v.x;

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

float2 intersect(float2 p0, float2 v0, float2 p1, float2 v1) {
    float2 intersection;

    float d = v0.x * v1.y - v0.y * v1.x;

    if (abs(d) < 0.0001f) {
        intersection = p0;
    }
    else
    {
        float n = (p1.x - p0.x) * v1.y - (p1.y - p0.y) * v1.x;
        float t = n / d;

        intersection = p0 + v0 * t;
    }

    return intersection;
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

        float2 vecPrev2d = calcNorm2d(posTmp2d, posPrevTmp2d);
        float2 vecNext2d = calcNorm2d(posTmp2d, posNextTmp2d);

        const float AAScale = 1.0f;

        float2 pos2dPrevC = posTmp2d - vecPrev2d * AAScale;
        float2 pos2dNextC = posTmp2d - vecNext2d * AAScale;

        vecPrev2d = rot90ccw(vecPrev2d);
        vecNext2d = rot90cw(vecNext2d);

        vecPrev2d *= input.aaDir;
        vecNext2d *= input.aaDir;

        //if (input.aaDir > 0.0)
        {
            pos2dPrevC += vecPrev2d * AAScale;
            pos2dNextC += vecNext2d * AAScale;

            vecPrev2d = rot90cw(vecPrev2d);
            vecNext2d = rot90ccw(vecNext2d);
        }

        // add better vector calculation, taking into accound needed thickness of AA - complete with intersect
        // TODO improve AA further for perspective projection and also for back faces(!)

        posTmp2d = intersect(pos2dPrevC, vecPrev2d, pos2dNextC, vecNext2d);

        //float2 aaVec = normalize(vecPrev2d + vecNext2d) * AAScale * input.aaDir;

        //posTmp2d += aaVec;

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