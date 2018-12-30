#include "FontAtlasFreetype.h"

FontAtlasFreetype::FontAtlasFreetype(std::vector<uint8_t> fontData)
    : fontData(std::move(fontData))
{}

FontAtlasFreetype::FontAtlasFreetype(Filesystem::IStream &stream)
    : FontAtlasFreetype(FontAtlasFreetype::ReadStream(stream))
{}

std::vector<uint8_t> FontAtlasFreetype::ReadStream(Filesystem::IStream &stream) {
    auto streamPos = stream.GetPosition();
    auto streamSize = stream.GetSize();

    std::vector<uint8_t> data;

    data.resize((size_t)(streamSize - streamPos));

    stream.Read(data.data(), (uint32_t)data.size());

    return data;
}