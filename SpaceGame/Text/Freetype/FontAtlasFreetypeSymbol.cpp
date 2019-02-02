#include "FontAtlasFreetypeSymbol.h"
#include "FontAtlasFreetypeBitmapLock.h"

FontAtlasFreetypeSymbol::FontAtlasFreetypeSymbol() {
}

FontAtlasSymbolMetrics FontAtlasFreetypeSymbol::GetMetrics() {
    FontAtlasSymbolMetrics metrics;

    return metrics;
}

uint32_t FontAtlasFreetypeSymbol::GetCharCode() {
    return -1;
}

std::shared_ptr<IFontAtlasBitmapLock> FontAtlasFreetypeSymbol::LockBitmap() {
    auto lk = std::make_shared<FontAtlasFreetypeBitmapLock>();

    return lk;
}