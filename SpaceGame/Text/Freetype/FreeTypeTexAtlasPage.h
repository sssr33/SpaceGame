#pragma once
#include "../IFontAtlasBitmapLock.h"
#include "../../Texture/TextureAtlasPage.h"

class FreeTypeTexAtlasPage {
public:
    FreeTypeTexAtlasPage(
        uint32_t bitmapId,
        const Structs::Size<uint32_t> &size,
        FontAtlasPixelFormat pixFmt = FontAtlasPixelFormat::Gray8);

    FontAtlasBitmapBytesInfo GetBytesInfo() const;

    TextureAtlasPage::AddTextureResult AddTexture(
        uint32_t charCode,
        const void *texMemory,
        const Structs::Size<uint32_t> &byteSize,
        uint32_t stride);

private:
    uint32_t bitmapId;
    uint32_t changeId;
    FontAtlasPixelFormat pixFmt;
    Structs::Size<uint32_t> size;
    TextureAtlasPage texAtlasPage;

    static TextureAtlasPage CreateTextureAtlasPage(FontAtlasPixelFormat pixFmt, Structs::Size<uint32_t> size);
};