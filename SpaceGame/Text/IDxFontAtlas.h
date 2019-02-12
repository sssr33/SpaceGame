#pragma once

#include <d3d11.h>
#include <memory>
#include <cstdint>
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
    virtual ID3D11Texture2D *GetTexture() = 0;
};

class IDxFontAtlas {
public:
    virtual ~IDxFontAtlas() = default;

    virtual std::shared_ptr<IDxSymbol> RenderSymbol(
        float fontHeight,
        uint32_t charCode) = 0;
};