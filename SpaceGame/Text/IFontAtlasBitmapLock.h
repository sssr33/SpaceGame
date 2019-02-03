#pragma once

#include <cstdint>
#include <vector>
#include <libhelpers/Structs.h>

enum class FontAtlasPixelFormat {
    Unknown,
    Gray8,
    RGB24,
    BGR24,
    RGB32,
    BGR32,
    RGBA32,
    BGRA32
};

struct FontAtlasBitmapBytesInfo {
    FontAtlasPixelFormat pixelFormat;
    const void *data;
    Structs::Size<uint32_t> size;
    uint32_t pitch;
    uint32_t bitmapId;
    uint32_t changeId;
};

struct FontAtlasBitmapSymbolInfo {
    Structs::Rect<uint32_t> dataRect; // in FontAtlasBitmapBytesInfo.data
    Structs::Point<uint32_t> partLeftTop; // // position inside complete char bitmap. If <IsBitmapPart> == false then it equals 0;0
    Structs::Size<uint32_t> charSize; // size of complete char bitmap. If <IsBitmapPart> == false then charSize == dataRect.size

    bool IsBitmapPart() const;
};

struct FontAtlasBitmapBytes {
    FontAtlasBitmapBytes(
        const FontAtlasBitmapBytesInfo &bytesInfo,
        const FontAtlasBitmapSymbolInfo &symbolInfo);

    const FontAtlasBitmapBytesInfo &GetBytesInfo() const;
    const FontAtlasBitmapSymbolInfo &GetSymbolInfo() const;

private:
    FontAtlasBitmapBytesInfo bytesInfo;
    FontAtlasBitmapSymbolInfo symbolInfo;
};

/*
Implementation must keep whole IFontAtlasBitmapBytes alive and immutable.
*/
class IFontAtlasBitmapLock {
public:
    virtual ~IFontAtlasBitmapLock() = default;

    virtual std::vector<FontAtlasBitmapBytes> GetBytes() = 0;
};