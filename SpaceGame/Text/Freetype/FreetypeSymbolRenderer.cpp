#include "FreetypeSymbolRenderer.h"
#include "HFreetype.h"

FreetypeSymbolRenderer::FreetypeSymbolRenderer(
    FT_F26Dot6 fontSize, FT_Face face,
    const Structs::Size<uint32_t> &texPageSize,
    std::shared_ptr<FreeTypeTexAtlasPage> lastTexPage)
    : bitmapId(0), fontSize(fontSize), face(face)
    , texPageSize(texPageSize), lastTexPage(std::move(lastTexPage))
{
    if (this->lastTexPage) {
        this->bitmapId = this->lastTexPage->GetBytesInfo().bitmapId;
    }
}

std::shared_ptr<FreeTypeTexAtlasPage> FreetypeSymbolRenderer::GetLastTexPage() const {
    return this->lastTexPage;
}

std::shared_ptr<IFontAtlasRenderedSymbol> FreetypeSymbolRenderer::Render(uint32_t charCode) {
    FT_Error error = FT_Err_Ok;

    error = FT_Set_Char_Size(this->face, 0, this->fontSize, 0, 0);
    H::Freetype::ThrowIfFailed(error);

    error = FT_Load_Char(this->face, charCode, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
    H::Freetype::ThrowIfFailed(error);

    error = FT_Render_Glyph(this->face->glyph, FT_RENDER_MODE_NORMAL);
    H::Freetype::ThrowIfFailed(error);

    auto parts = this->CreateParts(this->face->glyph->bitmap, charCode);
    auto metrics = this->CreateMetrics(this->face);

    auto symbol = std::make_shared<FontAtlasFreetypeSymbol>(metrics, charCode, std::move(parts));

    return symbol;
}

std::vector<FreetypeSymbolPart> FreetypeSymbolRenderer::CreateParts(const FT_Bitmap &bmp, uint32_t charCode) {
    std::vector<FreetypeSymbolPart> parts;

    if (!this->lastTexPage) {
        this->lastTexPage = std::make_shared<FreeTypeTexAtlasPage>(this->bitmapId, this->texPageSize);
        this->bitmapId++;
    }

    auto bmpByteSize = Structs::Size<uint32_t>(
        bmp.width,
        bmp.rows
        );

    auto addRes = this->lastTexPage->AddTexture(charCode, bmp.buffer, bmpByteSize, bmp.pitch);

    if (addRes) {
        FontAtlasBitmapSymbolInfo symbolInfo;

        symbolInfo.charSize = bmpByteSize;
        symbolInfo.partLeftTop = addRes.leftTop;

        symbolInfo.dataRect = Structs::Rect<uint32_t>(symbolInfo.partLeftTop, symbolInfo.charSize);

        FreetypeSymbolPart part;

        part.symbolInfo = symbolInfo;
        part.texPage = this->lastTexPage;

        parts.push_back(std::move(part));
    }
    else {
        this->lastTexPage = nullptr;
        parts = this->CreateParts(bmp, charCode);
    }

    return parts;
}

FontAtlasSymbolMetrics FreetypeSymbolRenderer::CreateMetrics(const FT_Face ftFace) {
    FontAtlasSymbolMetrics res;
    const auto &ftMetrics = ftFace->glyph->metrics;
    float ftHeight = (float)ftFace->height;

    res.rect.leftTop.x = (float)ftMetrics.vertBearingX / ftHeight;
    res.rect.leftTop.y = (float)ftMetrics.vertBearingY / ftHeight;
    res.rect.size.width = (float)ftMetrics.width / ftHeight;
    res.rect.size.height = (float)ftMetrics.height / ftHeight;

    res.advance.width = (float)ftMetrics.vertAdvance / ftHeight;
    res.advance.height = (float)ftMetrics.horiAdvance / ftHeight;

    return res;
}