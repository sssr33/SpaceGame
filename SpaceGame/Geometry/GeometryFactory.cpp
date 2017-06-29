#include "GeometryFactory.h"

#include <algorithm>
#include <Windows.h>
#include <libhelpers\HMath.h>

void GeometryFactory::CreateRectangle(float width, float height, float thickness, DataBuffer<DirectX::XMFLOAT2> pos, DataBuffer<DirectX::XMFLOAT2> aaVec, DataBuffer<uint16_t> idx) {
    GeometryFactory::CreateRectangle2(width, height, thickness, 0.3f);

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

void GeometryFactory::CreateRectangle2(float width, float height, float thickness, float roundness) {
    float lineLen = 1.0f - roundness;
    //float halfWidth = width / 2.0f;
    //float halfHeight = height / 2.0f;
    // 0 - outer; 1 - inner
    float halfWidth[2];
    float halfHeight[2];

    halfWidth[0] = (width + thickness) / 2.0f;
    halfWidth[1] = (width - thickness) / 2.0f;
    halfHeight[0] = (height + thickness) / 2.0f;
    halfHeight[1] = (height - thickness) / 2.0f;

    for (uint32_t i = 0; i < 2; i++) {
        DirectX::XMFLOAT2 ltStart; // begin of quadratic curve
        DirectX::XMFLOAT2 ltEnd; // end of quadratic curve; control point not used because sin/cos is used
        DirectX::XMFLOAT2 rtStart;

        ltStart.x = -halfWidth[i];
        ltStart.y = halfHeight[i] * lineLen;

        ltEnd.x = -halfWidth[i] * lineLen;
        ltEnd.y = halfHeight[i];

        rtStart.x = halfWidth[i] * lineLen;
        rtStart.y = halfHeight[i];

        if (!H::Math::NearEqual(ltStart, ltEnd)) {
            // make quadratic curve segment
        }

        if (!H::Math::NearEqual(ltEnd, rtStart)) {
            // make line segment
        }
    }
}

void GeometryFactory::GenLineIndexes(uint32_t topStart, uint32_t topCount, uint32_t bottomStart, uint32_t bottomCount, bool close) {
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

    std::vector<uint32_t> indices;

    const uint32_t offset[2][3] = {
        { 0, 0, 1 },
        { 1, 0, 0 }
    };
    const uint32_t select[2][3] = {
        { 0, 1, 0 },
        { 1, 0, 1 },
    };
    
    uint32_t triCount = (topCount + bottomCount) - (close ? 0 : 2);
    uint32_t idx[2] = { 0, 0 };
    uint32_t idxStart[2] = { bottomStart, topStart };
    uint32_t idxCount[2] = { bottomCount, topCount };

    // if bottom has more indices then its index must change slower, and get more triangles
    // fincrement used as increment for numerator
    bool bottomFidx = bottomCount > topCount;

    FractIdx findex[2] = {
        { 0, bottomFidx ? bottomCount : 1 },
        { 0, bottomFidx ? 1 : topCount }
    };
    uint32_t fincrement[2] = {
        bottomFidx ? topCount : 1,
        bottomFidx ? 1 : bottomCount
    };

    for (uint32_t tri = 0, i = 0; tri < triCount; tri++) {
        uint32_t control = i % 2;
        auto &sel = select[control];
        auto &off = offset[control];
        auto &fidx = findex[control];
        auto &finc = fincrement[control];

        for (uint32_t j = 0; j < 3; j++) {
            uint32_t idxVal = idxStart[sel[j]] + ((idx[sel[j]] + off[j]) % idxCount[sel[j]]);
            indices.push_back(idxVal);
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