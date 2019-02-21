#pragma once
#include "IDxSymbol.h"

#include <memory>
#include <cstdint>

class IDxFontAtlas {
public:
    virtual ~IDxFontAtlas() = default;

    virtual std::shared_ptr<IDxSymbol> RenderSymbol(
        float fontPixelHeight,
        uint32_t charCode) = 0;
};