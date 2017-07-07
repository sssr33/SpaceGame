#pragma once
#include "TexVertex.h"

#include <vector>
#include <DirectXMath.h>
#include <libhelpers\DataBuffer.h>

class GeometryFactory {
public:
    static void CreateRectangle(float width, float height, float thickness, DataBuffer<DirectX::XMFLOAT2> pos, DataBuffer<DirectX::XMFLOAT2> aaVec, DataBuffer<uint16_t> idx);

    static void CreateRectangle2(
        float width, float height,
        float thickness, float roundnessOuter, float roundnessInner,
        std::vector<DirectX::XMFLOAT2> &pos,
        std::vector<DirectX::XMFLOAT2> &adjPrev,
        std::vector<DirectX::XMFLOAT2> &adjNext,
        std::vector<float> &aaDir,
        std::vector<uint32_t> &indices);

    static void CreateRectangleFilled(
        float width, float height,
        float roundnessOuter,
        std::vector<DirectX::XMFLOAT2> &pos,
        std::vector<DirectX::XMFLOAT2> &adjPrev,
        std::vector<DirectX::XMFLOAT2> &adjNext,
        std::vector<float> &aaDir,
        std::vector<uint32_t> &indices);

private:

    struct GenLineIndexesParams {
        uint32_t topBegin;
        uint32_t topIdx;
        uint32_t topCount;
        uint32_t topTotalCount;
        uint32_t bottomBegin;
        uint32_t bottomIdx;
        uint32_t bottomCount;
        uint32_t bottomTotalCount;
        bool close;
        bool fill;
        std::vector<uint32_t> &indices;

        GenLineIndexesParams(std::vector<uint32_t> &indices, uint32_t topIdx = 0, uint32_t bottomIdx = 0);
    };

    static void GenLineIndexes(const GenLineIndexesParams &params);
};