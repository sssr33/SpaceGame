#pragma once

#include <DirectXMath.h>

struct TexVertex2D {
    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 tex;
};

struct RGBA8Color {
    union {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };
        uint32_t val;
    };
};

struct ColorVertex2D {
    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 adjPrev;
    DirectX::XMFLOAT2 adjNext;
    float aaDir;
    RGBA8Color color;
};