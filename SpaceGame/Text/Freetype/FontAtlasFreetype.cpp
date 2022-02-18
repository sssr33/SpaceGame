#include "FontAtlasFreetype.h"
#include "HFreetype.h"
#include "FontAtlasFreetypeSymbol.h"
#include "FreetypeSymbolRenderer.h"

#include <cassert>
#include <stdexcept>
#include <cmath>
#include <libhelpers/HMathCP.h>
#include <libhelpers/unique_ptr_extensions.h>

FontAtlasFreetype::FontAtlasFreetype(std::vector<uint8_t> fontData, const Structs::Size<uint32_t> &texPageSize)
    : fontData(std::move(fontData)), texPageSize(texPageSize)
{
    FT_Error error = FT_Err_Ok;

    error = FT_Init_FreeType(GetAddressOf(this->ftlib));
    H::Freetype::ThrowIfFailed(error);

    error = FT_New_Memory_Face(this->ftlib.get(), this->fontData.data(), (FT_Long)this->fontData.size(), 0, GetAddressOf(this->ftFace));
    H::Freetype::ThrowIfFailed(error);

    if (!(this->ftFace->face_flags & FT_FACE_FLAG_SCALABLE)) {
        // for now only scalable fonts are supported
        throw std::runtime_error("FontAtlasFreetype : only scalable fonts are supported");
    }
}

FontAtlasFreetype::FontAtlasFreetype(Filesystem::IStream &stream, const Structs::Size<uint32_t> &texPageSize)
    : FontAtlasFreetype(FontAtlasFreetype::ReadStream(stream), texPageSize)
{}

std::shared_ptr<IFontAtlasSymbol> FontAtlasFreetype::GetDefaultSymbol(uint32_t charCode) {
    throw std::runtime_error("not implemented");
}

std::shared_ptr<IFontAtlasSymbol> FontAtlasFreetype::GetSymbol(uint32_t charCode, float fontPixelHeight) {
    throw std::runtime_error("not implemented");
}

std::shared_ptr<IFontAtlasRenderedSymbol> FontAtlasFreetype::RenderSymbol(uint32_t charCode, float fontPixelHeight) {
    auto &symbolData = this->GetSymbolData(charCode);

    auto symbol = symbolData.GetSymbolData(fontPixelHeight);

    if (!symbol) {
        symbol = this->RenderSymbolInternal(charCode, fontPixelHeight);
        symbolData.SetSymbolData(fontPixelHeight, symbol);
    }

    return symbol;

    // check for pot size

    //auto ttt = H::MathCP::RoundUpPOT(33);

    ///*float ppem = (float)this->ftFace->units_per_EM / 65536.f;
    //float fntHeight1 = (float)this->ftFace->height / 64.0f;
    //float fntHeight = fntHeight1 * ppem;*/
    //float fntHeight2 = (float)this->ftFace->height / (float)this->ftFace->units_per_EM;
    //float fntHeight = (float)(this->ftFace->height) / (float)this->ftFace->units_per_EM;

    //float fontSize = fontPixelHeight / fntHeight;
    //float scaledH = fntHeight * fontSize;
    //float scaledH64 = scaledH * 64.f;
    //FT_F26Dot6 ftFontSize = (FT_F26Dot6)std::ceil(fontSize * 64.f);

    //error = FT_Set_Char_Size(this->ftFace.get(), 0, ftFontSize, 0, 0);
    //H::Freetype::ThrowIfFailed(error);

    //error = FT_Load_Char(this->ftFace.get(), charCode, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_NO_SCALE);
    //H::Freetype::ThrowIfFailed(error);

    //auto mnoscale = this->ftFace->glyph->metrics;

    //error = FT_Load_Char(this->ftFace.get(), charCode, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
    //H::Freetype::ThrowIfFailed(error);

    //auto mscaled = this->ftFace->glyph->metrics;

    //error = FT_Render_Glyph(this->ftFace->glyph, FT_RENDER_MODE_NORMAL);
    //H::Freetype::ThrowIfFailed(error);

    //auto bmpWidth = this->ftFace->glyph->bitmap.width;
    //auto bmpHeight = this->ftFace->glyph->bitmap.rows;

    //float tmp1 = (float)mnoscale.height / (float)this->ftFace->height;
    //float tmp2 = (float)mscaled.height / scaledH64;
    //float tmp3 = (float)bmpHeight / fontPixelHeight;

    //return tmp;
}

FontAtlasFreetype::SymbolData &FontAtlasFreetype::GetSymbolData(uint32_t charCode) {
    auto find = this->symbolData.find(charCode);

    if (find != this->symbolData.end()) {
        return find->second;
    }

    auto added = this->symbolData.insert(std::make_pair(charCode, SymbolData()));
    assert(added.second); // inserted

    return added.first->second;
}

std::shared_ptr<IFontAtlasRenderedSymbol> FontAtlasFreetype::RenderSymbolInternal(uint32_t charCode, float fontPixelHeight) {
    auto roundFntHeight = (float)FontAtlasFreetype::RoundFontHeight(fontPixelHeight);
    auto fntHeight = (float)(this->ftFace->height) / (float)this->ftFace->units_per_EM;
    float fontSize = fontPixelHeight / fntHeight;
    FT_F26Dot6 ftFontSize = (FT_F26Dot6)std::ceil(fontSize * 64.f);

    FreetypeSymbolRenderer renderer(ftFontSize, this->ftFace.get(), this->texPageSize, this->lastTexPage);

    auto symbol = renderer.Render(charCode);

    this->lastTexPage = renderer.GetLastTexPage();

    return symbol;
}

std::vector<uint8_t> FontAtlasFreetype::ReadStream(Filesystem::IStream &stream) {
    auto streamPos = stream.GetPosition();
    auto streamSize = stream.GetSize();

    std::vector<uint8_t> data;

    data.resize((size_t)(streamSize - streamPos));

    stream.Read(data.data(), (uint32_t)data.size());

    return data;
}

uint32_t FontAtlasFreetype::RoundFontHeight(float fontPixelHeight) {
    auto pot = H::MathCP::RoundUpPOT((size_t)std::ceil(fontPixelHeight));
    return pot;
}




std::shared_ptr<IFontAtlasRenderedSymbol> FontAtlasFreetype::SymbolData::GetSymbolData(float fontPixelHeight) const {
    auto idx = SymbolData::GetPOTSizeIdx(fontPixelHeight);

    if (idx >= this->potSize.size()) {
        return nullptr;
    }

    return this->potSize[idx].lock();
}

void FontAtlasFreetype::SymbolData::SetSymbolData(float fontPixelHeight, std::weak_ptr<IFontAtlasRenderedSymbol> data) {
    auto idx = SymbolData::GetPOTSizeIdx(fontPixelHeight);

    if (idx >= this->potSize.size()) {
        this->potSize.resize(idx + 1);
    }

    this->potSize[idx] = std::move(data);
}

size_t FontAtlasFreetype::SymbolData::GetPOTSizeIdx(float fontPixelHeight) {
    auto pot = FontAtlasFreetype::RoundFontHeight(fontPixelHeight);
    auto idx = (size_t)std::log2(pot);

    return idx;
}