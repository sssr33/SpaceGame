#include "FontAtlasFreetypeBitmapLock.h"

FontAtlasFreetypeBitmapLock::FontAtlasFreetypeBitmapLock(std::vector<FreetypeSymbolPart> parts)
    : parts(std::move(parts))
{}

std::vector<FontAtlasBitmapBytes> FontAtlasFreetypeBitmapLock::GetBytes() {
    std::vector<FontAtlasBitmapBytes> bytes;

    bytes.reserve(this->parts.size());

    for (auto &i : this->parts) {
        bytes.push_back(FontAtlasBitmapBytes(
            i.texPage->GetBytesInfo(), i.symbolInfo));
    }

    return bytes;
}