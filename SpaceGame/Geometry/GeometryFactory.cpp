#include "GeometryFactory.h"

#include <algorithm>
#include <Windows.h>

void GeometryFactory::CreateRectangle(float width, float height, float thickness, DataBuffer<DirectX::XMFLOAT2> pos, DataBuffer<uint16_t> idx) {
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

    pos.CopyData(verts);
    idx.CopyData(indices);
}