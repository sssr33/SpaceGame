#include "GeometryFactory.h"

#include <algorithm>
#include <Windows.h>
#include <libhelpers\HMath.h>

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

            posVecTmp = DirectX::XMVectorScale(posVecTmp, 2.0f);

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