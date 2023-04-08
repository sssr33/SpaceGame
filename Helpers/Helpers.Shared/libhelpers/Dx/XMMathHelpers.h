#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>

class XMMathHelpers {
public:
    static bool Equals(const DirectX::XMFLOAT4X4& a, const DirectX::XMFLOAT4X4& b);
};
