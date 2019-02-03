#pragma once
#include "FreeTypeTexAtlasPage.h"

#include <memory>

struct FreetypeSymbolPart {
    FontAtlasBitmapSymbolInfo symbolInfo;
    std::shared_ptr<FreeTypeTexAtlasPage> texPage;
};