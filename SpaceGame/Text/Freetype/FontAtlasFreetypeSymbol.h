#pragma once
#include "FreetypeSymbolPart.h"
#include "../IFontAtlasSymbol.h"

class FontAtlasFreetypeSymbol : public IFontAtlasRenderedSymbol {
public:
    FontAtlasFreetypeSymbol(
        const FontAtlasSymbolMetrics &metrics,
        uint32_t charCode,
        std::vector<FreetypeSymbolPart> parts);

    FontAtlasSymbolMetrics GetMetrics() override;
    uint32_t GetCharCode() override;
    std::shared_ptr<IFontAtlasBitmapLock> LockBitmap() override;

private:
    FontAtlasSymbolMetrics metrics;
    uint32_t charCode;
    std::vector<FreetypeSymbolPart> parts;
};