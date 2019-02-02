#pragma once
#include "../IFontAtlasSymbol.h"

class FontAtlasFreetypeSymbol : public IFontAtlasRenderedSymbol {
public:
    FontAtlasFreetypeSymbol();

    FontAtlasSymbolMetrics GetMetrics() override;
    uint32_t GetCharCode() override;
    std::shared_ptr<IFontAtlasBitmapLock> LockBitmap() override;
};