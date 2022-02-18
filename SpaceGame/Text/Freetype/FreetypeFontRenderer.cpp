#include "FreetypeFontRenderer.h"
#include "HFreetype.h"

#include <cassert>
#include <stdexcept>
#include <cmath>
#include <libhelpers/HMathCP.h>
#include <libhelpers/unique_ptr_extensions.h>

FreetypeFontRenderer::FreetypeFontRenderer(std::vector<uint8_t> fontData)
    : fontData(std::move(fontData))
{
    FT_Error error = FT_Err_Ok;

    error = FT_Init_FreeType(GetAddressOf(this->ftlib));
    H::Freetype::ThrowIfFailed(error);

    error = FT_New_Memory_Face(this->ftlib.get(), this->fontData.data(), (FT_Long)this->fontData.size(), 0, GetAddressOf(this->ftFace));
    H::Freetype::ThrowIfFailed(error);

    if (!(this->ftFace->face_flags & FT_FACE_FLAG_SCALABLE)) {
        // for now only scalable fonts are supported
        throw std::runtime_error("FreetypeFontRenderer : only scalable fonts are supported");
    }
}

FreetypeFontRenderer::FreetypeFontRenderer(Filesystem::IStream &stream)
    : FreetypeFontRenderer(FreetypeFontRenderer::ReadStream(stream))
{}

void FreetypeFontRenderer::Render(
    float fontPixelHeight,
    uint32_t charCode,
    IFontSinkBuilder &sinkBuilder)
{
    auto fntHeight = (float)(this->ftFace->height) / (float)this->ftFace->units_per_EM;
    float fontSize = fontPixelHeight / fntHeight;
    FT_F26Dot6 ftFontSize = (FT_F26Dot6)std::ceil(fontSize * 64.f);

    FT_Error error = FT_Err_Ok;

    error = FT_Set_Char_Size(this->ftFace.get(), 0, ftFontSize, 0, 0);
    H::Freetype::ThrowIfFailed(error);

    error = FT_Load_Char(this->ftFace.get(), charCode, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
    H::Freetype::ThrowIfFailed(error);

    error = FT_Render_Glyph(this->ftFace->glyph, FT_RENDER_MODE_NORMAL);
    H::Freetype::ThrowIfFailed(error);

    auto fmt = FontPixelFormat::Gray8;
    auto selectedIdx = sinkBuilder.SelectFormat(&fmt, 1);

    if (selectedIdx != 0) {
        throw std::runtime_error("FreetypeFontRenderer : only FontPixelFormat::Gray8 is supported");
    }

    auto &bitmap = this->ftFace->glyph->bitmap;
    auto bmpPixSize = Structs::Size<uint32_t>(bitmap.width, bitmap.rows);

    sinkBuilder.SetSize(bmpPixSize);

    auto metrics = this->GetMetrics();
    sinkBuilder.SetMetrics(metrics);

    auto &sink = sinkBuilder.GetSink();

    sink.SetSegment(
        Structs::Rect<uint32_t>(
            Structs::Point<uint32_t>(0, 0),
            bmpPixSize));

    PixelPlaneReadonly pixPlane;

    pixPlane.pitch = (uint32_t)bitmap.pitch;
    pixPlane.src = bitmap.buffer;

    sink.Write(&pixPlane, 1);
}

NormalizedMetrics FreetypeFontRenderer::GetMetrics() const {
    NormalizedMetrics res;
    const auto &ftMetrics = this->ftFace->glyph->metrics;
    float ftHeight = (float)this->ftFace->height;

    res.rect.leftTop.x = (float)ftMetrics.vertBearingX / ftHeight;
    res.rect.leftTop.y = (float)ftMetrics.vertBearingY / ftHeight;
    res.rect.size.width = (float)ftMetrics.width / ftHeight;
    res.rect.size.height = (float)ftMetrics.height / ftHeight;

    res.horzAdvance = (float)ftMetrics.vertAdvance / ftHeight;
    res.vertAdvance = (float)ftMetrics.horiAdvance / ftHeight;

    return res;
}

std::vector<uint8_t> FreetypeFontRenderer::ReadStream(Filesystem::IStream &stream) {
    auto streamPos = stream.GetPosition();
    auto streamSize = stream.GetSize();

    std::vector<uint8_t> data;

    data.resize((size_t)(streamSize - streamPos));

    stream.Read(data.data(), (uint32_t)data.size());

    return data;
}