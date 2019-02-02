#pragma once
#include "../IFontAtlasBitmapLock.h"

class FontAtlasFreetypeBitmapLock : public IFontAtlasBitmapLock {
public:
    FontAtlasFreetypeBitmapLock();
    ~FontAtlasFreetypeBitmapLock();

    FontAtlasBitmapBytes GetBytes() override;
};
