#include "XMMathHelpers.h"
#include "../HMath.h"

bool XMMathHelpers::Equals(const DirectX::XMFLOAT4X4& a, const DirectX::XMFLOAT4X4& b) {
    bool same = true;

    for (size_t y = 0; y < 4 && same; ++y) {
        for (size_t x = 0; x < 4 && same; ++x) {
            same = a.m[x][y] == b.m[x][y];
        }
    }

    return same;
}
