#pragma once

#include <cstdint>
#include <vector>
#include <libhelpers/Structs.h>

class TextureAtlasPage {
public:
    struct AddTextureResult {
        bool added;
        Structs::Point<uint32_t> leftTop;

        AddTextureResult();

        explicit operator bool() const;
    };

    TextureAtlasPage(const Structs::Size<uint32_t> &byteSize);

    Structs::Size<uint32_t> GetByteSize() const;

    const void *GetMemory() const;

    bool IsSpaceAvailable(const Structs::Size<uint32_t> &byteSize) const;
    bool IsSpaceAvailable(const Structs::Size<uint32_t> &byteSize, const Structs::Size<uint32_t> &byteBorderSize) const;

    AddTextureResult AddTexture(const void *texMemory, const Structs::Size<uint32_t> &byteSize);
    AddTextureResult AddTexture(const void *texMemory, const Structs::Size<uint32_t> &byteSize, uint32_t stride);
    AddTextureResult AddTexture(const void *texMemory, const Structs::Size<uint32_t> &byteSize, const Structs::Size<uint32_t> &byteBorderSize);
    AddTextureResult AddTexture(const void *texMemory, const Structs::Size<uint32_t> &byteSize, uint32_t stride, const Structs::Size<uint32_t> &byteBorderSize);

private:
    Structs::Size<uint32_t> byteSize;
    Structs::Point<uint32_t> curPt;
    uint32_t nextY;
    std::vector<uint8_t> memory;

    void IsSpaceAvailable(const Structs::Size<uint32_t> &byteSize, bool &curAvailable, bool &nextAvailable) const;
};