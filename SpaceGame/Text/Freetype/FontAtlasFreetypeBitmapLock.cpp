#include "FontAtlasFreetypeBitmapLock.h"

FontAtlasFreetypeBitmapLock::FontAtlasFreetypeBitmapLock() {
}

FontAtlasFreetypeBitmapLock::~FontAtlasFreetypeBitmapLock() {
}

FontAtlasBitmapBytes FontAtlasFreetypeBitmapLock::GetBytes() {
    FontAtlasBitmapBytes bytes{ FontAtlasBitmapBytesInfo(), FontAtlasBitmapSymbolInfo() };

    return bytes;
}