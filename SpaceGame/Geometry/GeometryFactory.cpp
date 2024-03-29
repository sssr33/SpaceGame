#include "GeometryFactory.h"

#include <cmath>
#include <algorithm>
#include <Windows.h>
#include <libhelpers\HMath.h>
#include <libhelpers\HSystem.h>

void GeometryFactory::CreateRectangle(float width, float height, float thickness, DataBuffer<DirectX::XMFLOAT2> pos, DataBuffer<DirectX::XMFLOAT2> aaVec, DataBuffer<uint16_t> idx) {
    float halfWidth = width / 2;
    float halfHeight = height / 2;
    float halfThick = thickness / 2;

    DirectX::XMFLOAT2 verts[] = {
        // outer
        DirectX::XMFLOAT2(-halfWidth - halfThick, halfHeight + halfThick),  // 0: left top
        DirectX::XMFLOAT2(halfWidth + halfThick, halfHeight + halfThick),   // 1: right top
        DirectX::XMFLOAT2(halfWidth + halfThick, -halfHeight - halfThick),  // 2: right bottom
        DirectX::XMFLOAT2(-halfWidth - halfThick, -halfHeight - halfThick), // 3: left bottom

        // inner
        DirectX::XMFLOAT2(-halfWidth + halfThick, halfHeight - halfThick),  // 4: left top
        DirectX::XMFLOAT2(-halfWidth + halfThick, -halfHeight + halfThick), // 5: left bottom
        DirectX::XMFLOAT2(halfWidth - halfThick, -halfHeight + halfThick),  // 6: right bottom
        DirectX::XMFLOAT2(halfWidth - halfThick, halfHeight - halfThick),   // 7: right top
    };

    uint16_t indices[] = {
        4, 0, 1,
        4, 1, 7,

        7, 1, 2,
        7, 2, 6,

        6, 2, 3,
        6, 3, 5,

        5, 3, 0,
        5, 0, 4
    };

    // geom = 0 - 3; aa = 8 - 11
    // 8; 0: left top
    // 9; 1: right top
    // 10; 2: right bottom
    // 11; 3: left bottom
    uint16_t indicesAAouter[] = {
        0, 8, 9,
        0, 9, 1,

        1, 9, 10,
        1, 10, 2,

        2, 10, 11,
        2, 11, 3,

        3, 11, 8,
        3, 8, 0
    };

    // geom = 4 - 7; aa = 12 - 15
    // 12; 4: left top
    // 13; 5: left bottom
    // 14; 6: right bottom
    // 15; 7: right top
    uint16_t indicesAAinner[] = {
        12, 4, 7,
        12, 7, 15,

        15, 7, 6,
        15, 6, 14,

        14, 6, 5,
        14, 5, 13,

        13, 5, 4,
        13, 4, 12
    };

    //for (size_t i = 0; i < ARRAYSIZE(verts); i++) {
    //    aaVec.Set(i, DirectX::XMFLOAT2(0.0f, 0.0f)); // geometry must have no aa vectors, to stay where it is
    //}


    for (size_t i = 0; i < aaVec.dataCount; i++) {
        aaVec.Set(i, DirectX::XMFLOAT2(0.0f, 0.0f)); // geometry must have no aa vectors, to stay where it is
    }

    pos.CopyData(verts);
    idx.CopyData(indices);

    pos.CopyData(verts, ARRAYSIZE(verts)); // same verts for aa
    idx.CopyData(indicesAAouter, ARRAYSIZE(indices));
    idx.CopyData(indicesAAinner, ARRAYSIZE(indices) * 2);

    auto calcAANormals = [](size_t start, size_t count, DataBuffer<DirectX::XMFLOAT2> pos, DataBuffer<DirectX::XMFLOAT2> aaVec) {
        for (size_t i = 0; i < count; i++) {
            size_t prevI = i == 0 ? count - 1 : i - 1;
            size_t nextI = i == (count - 1) ? 0 : i + 1;
            DirectX::XMFLOAT2 v0 = pos.Get(prevI + start);
            DirectX::XMFLOAT2 v1 = pos.Get(i + start);
            DirectX::XMFLOAT2 v2 = pos.Get(nextI + start);

            DirectX::XMFLOAT2 vec0Tmp(v1.x - v0.x, v1.y - v0.y);
            DirectX::XMFLOAT2 vec1Tmp(v2.x - v1.x, v2.y - v1.y);

            H::Math::Rotate90CCW(vec0Tmp.x, vec0Tmp.y);
            H::Math::Rotate90CCW(vec1Tmp.x, vec1Tmp.y);

            DirectX::XMVECTOR posVecTmp = DirectX::XMVectorSet(vec0Tmp.x + vec1Tmp.x, vec0Tmp.y + vec1Tmp.y, 0.0f, 0.0f);
            posVecTmp = DirectX::XMVector2Normalize(posVecTmp);

            /*posVecTmp = DirectX::XMVectorScale(posVecTmp, 0.5f);*/
            posVecTmp = DirectX::XMVectorScale(posVecTmp, 1.5f);

            DirectX::XMFLOAT2 posVec;

            DirectX::XMStoreFloat2(&posVec, posVecTmp);

            posVec.y = -posVec.y;

            aaVec.Set(i + start, posVec);
        }
    };

    calcAANormals(8, 4, pos, aaVec); // outer aa
    calcAANormals(12, 4, pos, aaVec); // inner aa

    //
}

// TODO comments(!!!), refactoring(!)
void GeometryFactory::CreateRectangle2(
    float width, float height,
    float thickness, float roundnessOuter, float roundnessInner,
    std::vector<DirectX::XMFLOAT2> &pos,
    std::vector<DirectX::XMFLOAT2> &texCoords,
    std::vector<DirectX::XMFLOAT2> &adjPrev,
    std::vector<DirectX::XMFLOAT2> &adjNext,
    std::vector<float> &aaDir,
    std::vector<uint32_t> &indices)
{
    //float halfWidth = width / 2.0f;
    //float halfHeight = height / 2.0f;
    // 0 - outer; 1 - inner
    float lineLen[2];
    float halfWidth[2];
    float halfHeight[2];
    uint32_t curIdx[2];
    uint32_t curIdxAA[2];
    uint32_t quadIdxCount[2];
    uint32_t lineIdxCount[2];
    uint32_t totalCount[2];
    DirectX::XMFLOAT2 ltStart[2]; // begin of quadratic curve
    DirectX::XMFLOAT2 ltEnd[2]; // end of quadratic curve; control point not used because sin/cos is used
    DirectX::XMFLOAT2 rtStart[2];
    const float vecScale[2] = { 1.0f, -1.0f };

    //GeometryFactory::GenLineIndexes(0, 4, 10, 2, false);
    //GeometryFactory::GenLineIndexes(10, 2, 0, 4, false);

    lineLen[0] = 1.0f - roundnessOuter;
    lineLen[1] = 1.0f - roundnessInner;

    halfWidth[0] = (width + thickness) / 2.0f;
    halfWidth[1] = (width - thickness) / 2.0f;
    halfHeight[0] = (height + thickness) / 2.0f;
    halfHeight[1] = (height - thickness) / 2.0f;

    for (uint32_t i = 0; i < 2; i++) {
        ltStart[i].x = -halfWidth[i];
        ltStart[i].y = halfHeight[i] * lineLen[i];

        ltEnd[i].x = -halfWidth[i] * lineLen[i];
        ltEnd[i].y = halfHeight[i];

        rtStart[i].x = halfWidth[i] * lineLen[i];
        rtStart[i].y = halfHeight[i];
    }

    // correcting inner shape so that it stay inside outer shape
    if (!H::Math::NearEqual(ltStart[0], ltEnd[0])) {
        DirectX::XMVECTOR midOuter, midInner;
        DirectX::XMFLOAT2 softT;
        float curRads = DirectX::XM_PIDIV2 * 0.5f;

        softT.x = 1.0f - std::cos(curRads);
        softT.y = std::sin(curRads);

        midOuter.ZF = midInner.ZF = 0.0f;
        midOuter.WF = midInner.WF = 1.0f;

        midOuter.XF = H::Math::Lerp(ltStart[0].x, ltEnd[0].x, softT.x);
        midOuter.YF = H::Math::Lerp(ltStart[0].y, ltEnd[0].y, softT.y);

        float outerDist = DirectX::XMVector2Length(DirectX::XMVectorSubtract(DirectX::g_XMIdentityR3, midOuter)).XF;
        outerDist -= thickness;

        float k = 1.0f;

        if (!H::Math::NearEqual(ltStart[1], ltEnd[1])) {
            midInner.XF = H::Math::Lerp(ltStart[1].x, ltEnd[1].x, softT.x);
            midInner.YF = H::Math::Lerp(ltStart[1].y, ltEnd[1].y, softT.y);
        }
        else if (!H::Math::NearEqual(ltEnd[1], rtStart[1])) {
            midInner.XF = ltEnd[1].x;
            midInner.YF = ltEnd[1].y;
        }

        float innerDist = DirectX::XMVector2Length(DirectX::XMVectorSubtract(DirectX::g_XMIdentityR3, midInner)).XF;

        if (innerDist > outerDist) {
            k = outerDist / innerDist;
        }

        // 6 multiplications
        /*ltStart[1].x *= k;
        ltStart[1].y *= k;

        ltEnd[1].x *= k;
        ltEnd[1].y *= k;

        rtStart[1].x *= k;
        rtStart[1].y *= k;*/

        // 5 multiplications, but more assgnments
        // for now this is used because it's the same algorithm as used previously in code
        // TODO move ltStart, ltEnd, rtStart calculations for function
        halfWidth[1] *= k;
        halfHeight[1] *= k;

        ltStart[1].x = -halfWidth[1];
        ltStart[1].y = halfHeight[1] * lineLen[1];

        ltEnd[1].x = -halfWidth[1] * lineLen[1];
        ltEnd[1].y = halfHeight[1];

        rtStart[1].x = halfWidth[1] * lineLen[1];
        rtStart[1].y = halfHeight[1];
    }

    for (uint32_t i = 0; i < 2; i++) {
        curIdx[i] = (uint32_t)pos.size();
        quadIdxCount[i] = 0;
        lineIdxCount[i] = 0;

        if (!H::Math::NearEqual(ltStart[i], ltEnd[i])) {
            // make quadratic curve segment

            for (float t = 0.0f; t < 1.0f; t += 1.0f / 20.0f, quadIdxCount[i]++) {
                DirectX::XMFLOAT2 softT;
                DirectX::XMFLOAT2 pt;
                float curRads = DirectX::XM_PIDIV2 * t;

                softT.x = 1.0f - std::cos(curRads);
                softT.y = std::sin(curRads);

                pt.x = H::Math::Lerp(ltStart[i].x, ltEnd[i].x, softT.x);
                pt.y = H::Math::Lerp(ltStart[i].y, ltEnd[i].y, softT.y);

                pos.push_back(pt);
                texCoords.push_back(GeometryFactory::GenRectTexCoordsFromPos(width, height, pos.back()));
                adjPrev.push_back(DirectX::XMFLOAT2(0.f, 0.f));
                adjNext.push_back(DirectX::XMFLOAT2(0.f, 0.f));
                aaDir.push_back(0.0f);
            }
        }

        if (!H::Math::NearEqual(ltEnd[i], rtStart[i])) {
            // make line segment

            for (float t = 0.0f; t < 1.0f; t += 1.0f, lineIdxCount[i]++) {
                DirectX::XMFLOAT2 pt;

                pt.x = H::Math::Lerp(ltEnd[i].x, rtStart[i].x, t);
                pt.y = H::Math::Lerp(ltEnd[i].y, rtStart[i].y, t);

                pos.push_back(pt);
                texCoords.push_back(GeometryFactory::GenRectTexCoordsFromPos(width, height, pos.back()));
                adjPrev.push_back(DirectX::XMFLOAT2(0.f, 0.f));
                adjNext.push_back(DirectX::XMFLOAT2(0.f, 0.f));
                aaDir.push_back(0.0f);
            }
        }

        size_t vertexCount = pos.size() - curIdx[i];

        auto mirrorVerterx = [&pos, &texCoords, &adjPrev, &adjNext, &aaDir, &width, &height, &curIdx, &i](size_t start, size_t vertexCount, bool mirrorX, bool mirrorY)
        {
            auto mirror = DirectX::XMFLOAT2(mirrorX ? -1.f : 1.f, mirrorY ? -1.f : 1.f);

            for (size_t j = start; j < vertexCount; j++) {
                auto vertex = pos[curIdx[i] + (vertexCount - 1 - (j - start))];

                vertex.x *= mirror.x;
                vertex.y *= mirror.y;

                pos.push_back(vertex);
                texCoords.push_back(GeometryFactory::GenRectTexCoordsFromPos(width, height, pos.back()));
                adjPrev.push_back(DirectX::XMFLOAT2(0.f, 0.f));
                adjNext.push_back(DirectX::XMFLOAT2(0.f, 0.f));
                aaDir.push_back(0.0f);
            }
        };

        mirrorVerterx(0, vertexCount, true, false);
        mirrorVerterx(vertexCount, vertexCount * 2, false, true);
        mirrorVerterx(vertexCount * 2, vertexCount * 3, true, false);

        totalCount[i] = static_cast<uint32_t>(pos.size() - curIdx[i]);
        curIdxAA[i] = static_cast<uint32_t>(pos.size());

        for (uint32_t j = 0; j < totalCount[i]; j++) {
            uint32_t jPrev = j == 0 ? totalCount[i] - 1 : j - 1;
            uint32_t jNext = j == (totalCount[i] - 1) ? 0 : j + 1;
            auto vPrev = pos[curIdx[i] + jPrev];
            auto vertex = pos[curIdx[i] + j];
            auto vNext = pos[curIdx[i] + jNext];
            pos.push_back(vertex);
            texCoords.push_back(texCoords[curIdx[i] + j]);

            //DirectX::XMFLOAT2 vec0Tmp(vertex.x - vPrev.x, vertex.y - vPrev.y);
            //DirectX::XMFLOAT2 vec1Tmp(vNext.x - vertex.x, vNext.y - vertex.y);

            //H::Math::Rotate90CCW(vec0Tmp.x, vec0Tmp.y);
            //H::Math::Rotate90CCW(vec1Tmp.x, vec1Tmp.y);

            //DirectX::XMVECTOR posVecTmp = DirectX::XMVectorSet(vec0Tmp.x + vec1Tmp.x, vec0Tmp.y + vec1Tmp.y, 0.0f, 0.0f);
            //posVecTmp = DirectX::XMVector2Normalize(posVecTmp);

            ////posVecTmp = DirectX::XMVectorScale(posVecTmp, 1.0f);
            ////posVecTmp = DirectX::XMVectorScale(posVecTmp, 1.5f);

            ////posVecTmp = DirectX::XMVectorScale(posVecTmp, 0.01f);

            //DirectX::XMFLOAT2 posVec;

            //DirectX::XMStoreFloat2(&posVec, posVecTmp);

            //posVec.y = -posVec.y;

            /*posVec.x *= vecScale[i];
            posVec.y *= vecScale[i];*/

            adjPrev.push_back(vPrev);
            adjNext.push_back(vNext);
            aaDir.push_back(vecScale[i]);
        }
    }

    GenLineIndexesParams params(indices);
    GenLineIndexesParams paramsOuterAA(indices);
    GenLineIndexesParams paramsInnerAA(indices);

    params.topBegin = curIdx[0];
    params.bottomBegin = curIdx[1];
    params.topTotalCount = totalCount[0];// curIdx[1] - curIdx[0];
    params.bottomTotalCount = totalCount[1];// (uint32_t)pos.size() - curIdx[1];
    params.close = true; // to connect segments

    paramsOuterAA.topBegin = curIdxAA[0];
    paramsOuterAA.bottomBegin = curIdx[0];
    paramsOuterAA.topTotalCount = totalCount[0];
    paramsOuterAA.bottomTotalCount = totalCount[0];
    paramsOuterAA.close = true;

    paramsInnerAA.topBegin = curIdx[1];
    paramsInnerAA.bottomBegin = curIdxAA[1];
    paramsInnerAA.topTotalCount = totalCount[1];
    paramsInnerAA.bottomTotalCount = totalCount[1];
    paramsInnerAA.close = true;

    for (uint32_t i = 0; i < 4; i++) {
        if (quadIdxCount[0] != 0 || quadIdxCount[1] != 0) {
            params.topCount = quadIdxCount[0];
            params.bottomCount = quadIdxCount[1];

            paramsOuterAA.topCount = quadIdxCount[0];
            paramsOuterAA.bottomCount = quadIdxCount[0];

            paramsInnerAA.topCount = quadIdxCount[1];
            paramsInnerAA.bottomCount = quadIdxCount[1];

            GeometryFactory::GenLineIndexes(params);
            GeometryFactory::GenLineIndexes(paramsOuterAA);
            GeometryFactory::GenLineIndexes(paramsInnerAA);

            params.topIdx += params.topCount;
            params.bottomIdx += params.bottomCount;

            paramsOuterAA.topIdx += paramsOuterAA.topCount;
            paramsOuterAA.bottomIdx += paramsOuterAA.bottomCount;

            paramsInnerAA.topIdx += paramsInnerAA.topCount;
            paramsInnerAA.bottomIdx += paramsInnerAA.bottomCount;
        }

        if (lineIdxCount[0] != 0 || lineIdxCount[1] != 0) {
            params.topCount = lineIdxCount[0];
            params.bottomCount = lineIdxCount[1];

            paramsOuterAA.topCount = lineIdxCount[0];
            paramsOuterAA.bottomCount = lineIdxCount[0];

            paramsInnerAA.topCount = lineIdxCount[1];
            paramsInnerAA.bottomCount = lineIdxCount[1];

            GeometryFactory::GenLineIndexes(params);
            GeometryFactory::GenLineIndexes(paramsOuterAA);
            GeometryFactory::GenLineIndexes(paramsInnerAA);

            params.topIdx += params.topCount;
            params.bottomIdx += params.bottomCount;

            paramsOuterAA.topIdx += paramsOuterAA.topCount;
            paramsOuterAA.bottomIdx += paramsOuterAA.bottomCount;

            paramsInnerAA.topIdx += paramsInnerAA.topCount;
            paramsInnerAA.bottomIdx += paramsInnerAA.bottomCount;
        }
    }

    int stop = 234;
}

void GeometryFactory::CreateRectangleFilled(
    float width, float height,
    float roundnessOuter,
    std::vector<DirectX::XMFLOAT2> &pos,
    std::vector<DirectX::XMFLOAT2> &texCoords,
    std::vector<DirectX::XMFLOAT2> &adjPrev,
    std::vector<DirectX::XMFLOAT2> &adjNext,
    std::vector<float> &aaDir,
    std::vector<uint32_t> &indices)
{
    float lineLen;
    float halfWidth;
    float halfHeight;
    uint32_t curIdx;
    uint32_t curIdxAA;
    uint32_t quadIdxCount;
    uint32_t lineIdxCount;
    uint32_t totalCount;
    DirectX::XMFLOAT2 ltStart; // begin of quadratic curve
    DirectX::XMFLOAT2 ltEnd; // end of quadratic curve; control point not used because sin/cos is used
    DirectX::XMFLOAT2 rtStart;
    const float vecScale = 1.0f;

    lineLen = 1.0f - roundnessOuter;

    halfWidth = width / 2.0f;
    halfHeight = height / 2.0f;

    ltStart.x = -halfWidth;
    ltStart.y = halfHeight * lineLen;

    ltEnd.x = -halfWidth * lineLen;
    ltEnd.y = halfHeight;

    rtStart.x = halfWidth * lineLen;
    rtStart.y = halfHeight;

    curIdx = (uint32_t)pos.size();
    quadIdxCount = 0;
    lineIdxCount = 0;

    if (!H::Math::NearEqual(ltStart, ltEnd)) {
        // make quadratic curve segment

        for (float t = 0.0f; t < 1.0f; t += 1.0f / 20.0f, quadIdxCount++) {
            DirectX::XMFLOAT2 softT;
            DirectX::XMFLOAT2 pt;
            float curRads = DirectX::XM_PIDIV2 * t;

            softT.x = 1.0f - std::cos(curRads);
            softT.y = std::sin(curRads);

            pt.x = H::Math::Lerp(ltStart.x, ltEnd.x, softT.x);
            pt.y = H::Math::Lerp(ltStart.y, ltEnd.y, softT.y);

            pos.push_back(pt);
            texCoords.push_back(GeometryFactory::GenRectTexCoordsFromPos(width, height, pos.back()));
            adjPrev.push_back(DirectX::XMFLOAT2(0.f, 0.f));
            adjNext.push_back(DirectX::XMFLOAT2(0.f, 0.f));
            aaDir.push_back(0.0f);
        }
    }

    if (!H::Math::NearEqual(ltEnd, rtStart)) {
        // make line segment

        for (float t = 0.0f; t < 1.0f; t += 1.0f, lineIdxCount++) {
            DirectX::XMFLOAT2 pt;

            pt.x = H::Math::Lerp(ltEnd.x, rtStart.x, t);
            pt.y = H::Math::Lerp(ltEnd.y, rtStart.y, t);

            pos.push_back(pt);
            texCoords.push_back(GeometryFactory::GenRectTexCoordsFromPos(width, height, pos.back()));
            adjPrev.push_back(DirectX::XMFLOAT2(0.f, 0.f));
            adjNext.push_back(DirectX::XMFLOAT2(0.f, 0.f));
            aaDir.push_back(0.0f);
        }
    }

    size_t vertexCount = pos.size() - curIdx;

    auto mirrorVerterx = [&pos, &texCoords, &adjPrev, &adjNext, &aaDir, &width, &height, &curIdx](size_t start, size_t vertexCount, bool mirrorX, bool mirrorY)
    {
        auto mirror = DirectX::XMFLOAT2(mirrorX ? -1.f : 1.f, mirrorY ? -1.f : 1.f);

        for (size_t j = start; j < vertexCount; j++) {
            auto vertex = pos[curIdx + (vertexCount - 1 - (j - start))];

            vertex.x *= mirror.x;
            vertex.y *= mirror.y;

            pos.push_back(vertex);
            texCoords.push_back(GeometryFactory::GenRectTexCoordsFromPos(width, height, pos.back()));
            adjPrev.push_back(DirectX::XMFLOAT2(0.f, 0.f));
            adjNext.push_back(DirectX::XMFLOAT2(0.f, 0.f));
            aaDir.push_back(0.0f);
        }
    };

    mirrorVerterx(0, vertexCount, true, false);
    mirrorVerterx(vertexCount, vertexCount * 2, false, true);
    mirrorVerterx(vertexCount * 2, vertexCount * 3, true, false);

    totalCount = static_cast<uint32_t>(pos.size() - curIdx);
    curIdxAA = static_cast<uint32_t>(pos.size());

    for (uint32_t j = 0; j < totalCount; j++) {
        uint32_t jPrev = j == 0 ? totalCount - 1 : j - 1;
        uint32_t jNext = j == (totalCount - 1) ? 0 : j + 1;
        auto vPrev = pos[curIdx + jPrev];
        auto vertex = pos[curIdx + j];
        auto vNext = pos[curIdx + jNext];
        pos.push_back(vertex);
        texCoords.push_back(texCoords[curIdx + j]);

        adjPrev.push_back(vPrev);
        adjNext.push_back(vNext);
        aaDir.push_back(vecScale);
    }

    uint32_t centerIdx = static_cast<uint32_t>(pos.size());

    pos.push_back(DirectX::XMFLOAT2(0.0f, 0.0f));
    texCoords.push_back(DirectX::XMFLOAT2(0.5f, 0.5f));
    adjPrev.push_back(DirectX::XMFLOAT2(0.f, 0.f));
    adjNext.push_back(DirectX::XMFLOAT2(0.f, 0.f));
    aaDir.push_back(0.0f);

    GenLineIndexesParams params(indices);
    GenLineIndexesParams paramsOuterAA(indices);

    params.topBegin = curIdx;
    params.bottomBegin = centerIdx;
    params.topTotalCount = totalCount;// curIdx[1] - curIdx[0];
    params.topCount = params.topTotalCount;
    params.bottomTotalCount = 1;// (uint32_t)pos.size() - curIdx[1];
    params.bottomCount = params.bottomTotalCount;
    params.close = true; // to connect segments
    params.fill = true;

    paramsOuterAA.topBegin = curIdxAA;
    paramsOuterAA.bottomBegin = curIdx;
    paramsOuterAA.topTotalCount = totalCount;
    paramsOuterAA.topCount = paramsOuterAA.topTotalCount;
    paramsOuterAA.bottomTotalCount = totalCount;
    paramsOuterAA.bottomCount = paramsOuterAA.bottomTotalCount;
    paramsOuterAA.close = true;

    GeometryFactory::GenLineIndexes(params);
    GeometryFactory::GenLineIndexes(paramsOuterAA);
}

GeometryFactory::GenLineIndexesParams::GenLineIndexesParams(std::vector<uint32_t> &indices, uint32_t topIdx, uint32_t bottomIdx)
    : indices(indices), topIdx(topIdx), bottomIdx(bottomIdx), fill(false)
{}

void GeometryFactory::GenLineIndexes(const GenLineIndexesParams &params) {
    struct FractIdx {
        uint32_t num;
        uint32_t den;

        FractIdx simplified() const {
            FractIdx res;
            FractIdx tmp = *this;

            while (true) {
                if (tmp.num > tmp.den) {
                    tmp.num = tmp.num - tmp.den;
                }
                else if (tmp.num < tmp.den) {
                    tmp.den = tmp.den - tmp.num;
                }
                else if (tmp.num == tmp.den) {
                    break;
                }
            }

            if (tmp.num) {
                res.num = this->num / tmp.num;
                res.den = this->den / tmp.num;
            }

            return res;
        }
    };

    const uint32_t offset[2][3] = {
        { 0, 0, 1 },
        { 1, 0, 0 }
    };
    const uint32_t select[2][3] = {
        { 0, 1, 0 },
        { 1, 0, 1 },
    };

    uint32_t triCount = (params.topCount + params.bottomCount);
    if (params.close) {
        if (params.fill) {
            triCount -= 1;
        }
        // to close not filled geometry we need 2 additional triangles
    }
    else {
        triCount -= 2;
    }

    uint32_t idx[2] = { params.bottomIdx, params.topIdx };
    uint32_t idxStart[2] = { params.bottomBegin, params.topBegin };
    uint32_t idxCount[2] = { params.bottomTotalCount, params.topTotalCount };

    // if bottom has more indices then its index must change slower, and get more triangles
    // fincrement used as increment for numerator
    bool bottomFidx = params.bottomCount > params.topCount;

    FractIdx findex[2] = {
        { 0, bottomFidx ? params.bottomCount : 1 },
        { 0, bottomFidx ? 1 : params.topCount }
    };
    uint32_t fincrement[2] = {
        bottomFidx ? params.topCount : 1,
        bottomFidx ? 1 : params.bottomCount
    };

    // bottomFidx ? 0 : 1 to generate indexes from where more vertices is
    // if we will start from less vertices side algrorithm will create wrong triangles at the end
    for (uint32_t tri = 0, i = bottomFidx ? 0 : 1; tri < triCount; tri++) {
        uint32_t control = i % 2;
        auto &sel = select[control];
        auto &off = offset[control];
        auto &fidx = findex[control];
        auto &finc = fincrement[control];

        for (uint32_t j = 0; j < 3; j++) {
            uint32_t idxVal = idxStart[sel[j]] + ((idx[sel[j]] + off[j]) % idxCount[sel[j]]);
            params.indices.push_back(idxVal);
        }

        idx[control]++;

        fidx.num += finc;
        if (fidx.num >= fidx.den) {
            fidx.num %= fidx.den;
            i++;
        }
    }

    int stop = 324;
}

DirectX::XMFLOAT2 GeometryFactory::GenRectTexCoordsFromPos(float width, float height, const DirectX::XMFLOAT2& pos) {
    float xMin = -width * 0.5f;
    float yMin = -height * 0.5f;

    DirectX::XMFLOAT2 tex = {
        std::clamp((pos.x - xMin) / width, 0.f, 1.f),
        std::clamp((-pos.y - yMin) / height, 0.f, 1.f)
    };

    return tex;
}
