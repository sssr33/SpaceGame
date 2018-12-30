#pragma once
#include "IFontAtlas.h"

#include <memory>
#include <cstdint>
#include <vector>
#include <libhelpers/Filesystem/IStreamFS.h>

class FontAtlasFreetype : public IFontAtlas {
public:
    FontAtlasFreetype(std::vector<uint8_t> fontData);
    FontAtlasFreetype(Filesystem::IStream &stream);

private:
    std::vector<uint8_t> fontData;

    static std::vector<uint8_t> ReadStream(Filesystem::IStream &stream);
};