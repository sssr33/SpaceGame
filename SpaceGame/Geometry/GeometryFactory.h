#pragma once
#include "TexVertex.h"

#include <vector>
#include <DirectXMath.h>
#include <libhelpers\DataBuffer.h>

class GeometryFactory {
public:
    static void CreateRectangle(float width, float height, float thickness, DataBuffer<DirectX::XMFLOAT2> pos, DataBuffer<DirectX::XMFLOAT2> aaVec, DataBuffer<uint16_t> idx);
};