#include "FreeTypeTexAtlasPage.h"

#include <stdexcept>
#include <libhelpers/HData.h>

FreeTypeTexAtlasPage::FreeTypeTexAtlasPage(
    uint32_t bitmapId,
    const Structs::Size<uint32_t> &size,
    FontAtlasPixelFormat pixFmt)
    : bitmapId(bitmapId), changeId(0), pixFmt(pixFmt), size(size)
    , texAtlasPage(FreeTypeTexAtlasPage::CreateTextureAtlasPage(pixFmt, size))
{}

FontAtlasBitmapBytesInfo FreeTypeTexAtlasPage::GetBytesInfo() const {
    FontAtlasBitmapBytesInfo info;

    info.bitmapId = this->bitmapId;
    info.changeId = this->changeId;
    info.data = this->texAtlasPage.GetMemory();
    info.pitch = this->texAtlasPage.GetByteSize().width;
    info.pixelFormat = this->pixFmt;
    info.size = this->size;

    return info;
}

TextureAtlasPage::AddTextureResult FreeTypeTexAtlasPage::AddTexture(
    uint32_t charCode,
    const void *texMemory,
    const Structs::Size<uint32_t> &byteSize,
    uint32_t stride)
{
    const auto borderSize = Structs::Size<uint32_t>(1, 1);

    auto res = this->texAtlasPage.AddTexture(texMemory, byteSize, stride, borderSize);

    if (res) {
        this->changeId = H::Data::CombineHash(this->changeId, charCode);
    }

    return res;
}

TextureAtlasPage FreeTypeTexAtlasPage::CreateTextureAtlasPage(FontAtlasPixelFormat pixFmt, Structs::Size<uint32_t> size) {
    uint32_t pixByteSize = 0;

    switch (pixFmt) {
    case FontAtlasPixelFormat::Gray8:
        pixByteSize = 1;
        break;
    case FontAtlasPixelFormat::RGB24:
    case FontAtlasPixelFormat::BGR24:
        pixByteSize = 3;
        break;
    case FontAtlasPixelFormat::RGB32:
    case FontAtlasPixelFormat::BGR32:
    case FontAtlasPixelFormat::RGBA32:
    case FontAtlasPixelFormat::BGRA32:
        pixByteSize = 4;
        break;
    default: {
        auto msg = std::string() + "Unsupporter pixel format == FontAtlasPixelFormat(" + std::to_string((uint32_t)pixFmt) + ")";
        throw std::runtime_error(std::move(msg));
        break;
    }
    }

    auto byteSize = Structs::Size<uint32_t>(size.width * pixByteSize, size.height);

    auto res = TextureAtlasPage(byteSize);
    return res;
}