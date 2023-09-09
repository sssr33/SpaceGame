#pragma once
#include "RGBA8Color.h"

#include <DirectXMath.h>

struct TexVertex2D {
    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 tex;
};

struct ColorVertex2D {
    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 tex;
    DirectX::XMFLOAT2 adjPrev;
    DirectX::XMFLOAT2 adjNext;
    float aaDir;
    RGBA8Color color;
};
