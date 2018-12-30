#pragma once
#include "IFontAtlasBitmapLock.h"

#include <cstdint>
#include <memory>

// Metrics normalized relative to font height
struct IFontAtlasSymbolMetrics {
    float left;
    float top;
    float width;
    float heigth;
    float xAdvance;
    float yAdvance;
};

class IFontAtlasSymbol {
public:
    virtual ~IFontAtlasSymbol() = default;

    virtual IFontAtlasSymbolMetrics GetMetrics() = 0;
    virtual uint32_t GetCharCode() = 0;
};

class IFontAtlasRenderedSymbol : public IFontAtlasSymbol {
public:
    virtual ~IFontAtlasRenderedSymbol() = default;

    // Gets accessor to symbol pixels.
    // It's better to keep it alive for short period of time to give IFontAtlas opportunity to make some optimizations if it can.
    virtual std::shared_ptr<IFontAtlasBitmapLock> LockBitmap() = 0;
};