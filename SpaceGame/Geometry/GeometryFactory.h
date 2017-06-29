#pragma once
#include "TexVertex.h"

#include <vector>
#include <DirectXMath.h>
#include <libhelpers\DataBuffer.h>

class GeometryFactory {
public:
    static void CreateRectangle(float width, float height, float thickness, DataBuffer<DirectX::XMFLOAT2> pos, DataBuffer<DirectX::XMFLOAT2> aaVec, DataBuffer<uint16_t> idx);

    static void CreateRectangle2(float width, float height, float thickness, float roundness);

private:
    static void GenLineIndexes(uint32_t topStart, uint32_t topCount, uint32_t bottomStart, uint32_t bottomCount, bool close);
};