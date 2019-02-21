#pragma once

#include <d3d11.h>
#include <libhelpers/Structs.h>

struct DxSymbolGeometry {
    Structs::Rect<float> texRect;
    Structs::Rect<float> geomRect;
    Structs::Size<float> advance;
};

class IDxSymbol {
public:
    virtual ~IDxSymbol() = default;

    virtual DxSymbolGeometry GetGeometry() = 0;
    virtual void GetTexture(ID3D11Texture2D **tex) = 0;
};