#pragma once
#include "FreetypeSymbolPart.h"
#include "../IFontAtlasBitmapLock.h"

class FontAtlasFreetypeBitmapLock : public IFontAtlasBitmapLock {
public:
    FontAtlasFreetypeBitmapLock(
        std::vector<FreetypeSymbolPart> parts);

    std::vector<FontAtlasBitmapBytes> GetBytes() override;

private:
    std::vector<FreetypeSymbolPart> parts;
};
