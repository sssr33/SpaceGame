#pragma once
#include "IFontAtlasSymbol.h"

#include <memory>
#include <cstdint>
#include <vector>

class IFontAtlas {
public:
    virtual ~IFontAtlas() = default;

    // Gets IFontAtlasSymbol with default metrics
    virtual std::shared_ptr<IFontAtlasSymbol> GetDefaultSymbol(uint32_t charCode) = 0;

    // Gets IFontAtlasSymbol with metrics for fontPixelHeight
    virtual std::shared_ptr<IFontAtlasSymbol> GetSymbol(uint32_t charCode, float fontPixelHeight) = 0;

    // Gets IFontAtlasRenderedSymbol with metrics and bitmap for fontPixelHeight
    virtual std::shared_ptr<IFontAtlasRenderedSymbol> RenderSymbol(uint32_t charCode, float fontPixelHeight) = 0;
};