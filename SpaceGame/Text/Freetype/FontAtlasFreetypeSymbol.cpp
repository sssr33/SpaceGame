#include "FontAtlasFreetypeSymbol.h"
#include "FontAtlasFreetypeBitmapLock.h"

FontAtlasFreetypeSymbol::FontAtlasFreetypeSymbol(
    const FontAtlasSymbolMetrics &metrics,
    uint32_t charCode,
    std::vector<FreetypeSymbolPart> parts)
    : metrics(metrics), charCode(charCode)
    , parts(std::move(parts))
{}

FontAtlasSymbolMetrics FontAtlasFreetypeSymbol::GetMetrics() {
    return this->metrics;
}

uint32_t FontAtlasFreetypeSymbol::GetCharCode() {
    return this->charCode;
}

std::shared_ptr<IFontAtlasBitmapLock> FontAtlasFreetypeSymbol::LockBitmap() {
    auto lk = std::make_shared<FontAtlasFreetypeBitmapLock>(this->parts);
    return lk;
}