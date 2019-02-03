#pragma once
#include "FreetypeSafe.h"
#include "FreetypeSymbolPart.h"
#include "FontAtlasFreetypeSymbol.h"
#include "FreeTypeTexAtlasPage.h"

class FreetypeSymbolRenderer {
public:
    FreetypeSymbolRenderer(
        FT_F26Dot6 fontSize, FT_Face face,
        const Structs::Size<uint32_t> &texPageSize,
        std::shared_ptr<FreeTypeTexAtlasPage> lastTexPage);

    std::shared_ptr<FreeTypeTexAtlasPage> GetLastTexPage() const;

    std::shared_ptr<IFontAtlasRenderedSymbol> Render(uint32_t charCode);

private:
    uint32_t bitmapId;
    FT_F26Dot6 fontSize;
    FT_Face face;
    Structs::Size<uint32_t> texPageSize;
    std::shared_ptr<FreeTypeTexAtlasPage> lastTexPage;

    std::vector<FreetypeSymbolPart> CreateParts(const FT_Bitmap &bmp, uint32_t charCode);
    static FontAtlasSymbolMetrics CreateMetrics(const FT_Face ftFace);
};