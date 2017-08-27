#pragma once

#include <vector>
#include <cstdint>
#include <libhelpers\Macros.h>

class TextureAtlasPage {
public:
    NO_COPY(TextureAtlasPage);

    TextureAtlasPage();
    TextureAtlasPage(TextureAtlasPage &&other);
    ~TextureAtlasPage();

    TextureAtlasPage &operator=(TextureAtlasPage &&other);

private:
    std::vector<uint8_t> memory;
};