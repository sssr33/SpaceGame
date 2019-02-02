#include "IFontAtlasBitmapLock.h"

#include <cassert>

bool FontAtlasBitmapSymbolInfo::IsBitmapPart() const {
    bool part = this->dataRect.size != this->charSize;

    assert(part || (this->partLeftTop == Structs::PointU(0, 0)));
    assert(part || (this->dataRect.size == this->charSize));

    return part;
}




FontAtlasBitmapBytes::FontAtlasBitmapBytes(
    const FontAtlasBitmapBytesInfo &bytesInfo,
    const FontAtlasBitmapSymbolInfo &symbolInfo)
    : bytesInfo(bytesInfo)
    , symbolInfo(symbolInfo)
{}

const FontAtlasBitmapBytesInfo &FontAtlasBitmapBytes::GetBytesInfo() const {
    return this->bytesInfo;
}

const FontAtlasBitmapSymbolInfo &FontAtlasBitmapBytes::GetSymbolInfo() const {
    return this->symbolInfo;
}