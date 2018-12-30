#pragma once

#include <cstdint>
#include <vector>

class TextureAtlasPage {
public:
    struct AddTextureResult {
        bool added;
        uint32_t x;
        uint32_t y;

        AddTextureResult();

        explicit operator bool() const;
    };

    TextureAtlasPage(uint32_t byteWidth, uint32_t byteHeight);

    uint32_t GetByteWidth() const;
    uint32_t GetByteHeight() const;

    const void *GetMemory() const;

    bool IsSpaceAvailable(uint32_t byteWidth, uint32_t byteHeight) const;
    bool IsSpaceAvailable(uint32_t byteWidth, uint32_t byteHeight, uint32_t borderX, uint32_t borderY) const;

    AddTextureResult AddTexture(const void *texMemory, uint32_t byteWidth, uint32_t byteHeight);
    AddTextureResult AddTexture(const void *texMemory, uint32_t byteWidth, uint32_t byteHeight, uint32_t stride);
    AddTextureResult AddTexture(const void *texMemory, uint32_t byteWidth, uint32_t byteHeight, uint32_t borderX, uint32_t borderY);
    AddTextureResult AddTexture(const void *texMemory, uint32_t byteWidth, uint32_t byteHeight, uint32_t stride, uint32_t borderX, uint32_t borderY);

private:
    uint32_t byteWidth;
    uint32_t byteHeight;
    uint32_t curX;
    uint32_t curY;
    uint32_t nextY;
    std::vector<uint8_t> memory;

    void IsSpaceAvailable(uint32_t byteWidth, uint32_t byteHeight, bool &curAvailable, bool &nextAvailable) const;
};