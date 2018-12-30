#include "TextureAtlasPage.h"

#include <cassert>
#include <algorithm>

TextureAtlasPage::TextureAtlasPage(uint32_t byteWidth, uint32_t byteHeight)
    : byteWidth(byteWidth), byteHeight(byteHeight)
    , curX(0), curY(0), nextY(0)
    , memory(byteWidth * byteHeight)
{}

uint32_t TextureAtlasPage::GetByteWidth() const {
    return this->byteWidth;
}

uint32_t TextureAtlasPage::GetByteHeight() const {
    return this->byteHeight;
}

const void *TextureAtlasPage::GetMemory() const {
    return this->memory.data();
}

bool TextureAtlasPage::IsSpaceAvailable(uint32_t byteWidth, uint32_t byteHeight) const {
    bool curAvailable = false;
    bool nextAvailable = false;

    this->IsSpaceAvailable(byteWidth, byteHeight, curAvailable, nextAvailable);

    bool available = curAvailable || nextAvailable;

    return curAvailable;
}

bool TextureAtlasPage::IsSpaceAvailable(uint32_t byteWidth, uint32_t byteHeight, uint32_t borderX, uint32_t borderY) const {
    return this->IsSpaceAvailable(byteWidth + borderX, byteHeight + borderY);
}

TextureAtlasPage::AddTextureResult TextureAtlasPage::AddTexture(const void *texMemory, uint32_t byteWidth, uint32_t byteHeight) {
    return this->AddTexture(texMemory, byteWidth, byteHeight, 0, 0);
}

TextureAtlasPage::AddTextureResult TextureAtlasPage::AddTexture(const void *texMemory, uint32_t byteWidth, uint32_t byteHeight, uint32_t stride) {
    return this->AddTexture(texMemory, byteWidth, byteHeight, stride, 0, 0);
}

TextureAtlasPage::AddTextureResult TextureAtlasPage::AddTexture(const void *texMemory, uint32_t byteWidth, uint32_t byteHeight, uint32_t borderX, uint32_t borderY) {
    return this->AddTexture(texMemory, byteWidth, byteHeight, byteWidth, borderX, borderY);
}

TextureAtlasPage::AddTextureResult TextureAtlasPage::AddTexture(const void *texMemory, uint32_t byteWidth, uint32_t byteHeight, uint32_t stride, uint32_t borderX, uint32_t borderY) {
    bool curAvailable = false;
    bool nextAvailable = false;
    AddTextureResult addResult;

    this->IsSpaceAvailable(byteWidth + borderX, byteHeight + borderY, curAvailable, nextAvailable);

    if (!curAvailable && !nextAvailable) {
        return addResult;
    }
    else if (!curAvailable) {
        assert(nextAvailable);

        this->curX = 0;
        this->curY = this->nextY;
    }

    auto texMemBytes = static_cast<const uint8_t*>(texMemory);

    for (uint32_t y = 0; y < byteHeight; y++) {
        auto dstLine = &this->memory[this->curX + (this->curY + y) * this->byteWidth];
        auto srcLine = &texMemBytes[stride * y];

        std::memcpy(dstLine, srcLine, byteWidth);
    }

    addResult.added = true;
    addResult.x = this->curX;
    addResult.y = this->curY;

    this->curX += byteWidth + borderX;
    this->nextY = (std::max)(this->nextY, this->curY + byteHeight + borderY);

    return addResult;
}

void TextureAtlasPage::IsSpaceAvailable(uint32_t byteWidth, uint32_t byteHeight, bool &curAvailable, bool &nextAvailable) const {
    auto curFreeWidth = this->curX - this->byteWidth;
    auto curFreeHeight = this->curY - this->byteHeight;
    auto nextFreeWidth = this->byteWidth;
    auto nextFreeHeight = this->nextY - this->byteHeight;

    bool curWidthAvailable = curFreeWidth >= byteWidth;
    bool curHeightAvailable = curFreeHeight >= byteHeight;
    bool nextWidthAvailable = nextFreeWidth >= byteWidth;
    bool nextHeightAvailable = nextFreeHeight >= byteHeight;

    curAvailable = curWidthAvailable && curHeightAvailable;
    nextAvailable = nextWidthAvailable && nextHeightAvailable;
}




TextureAtlasPage::AddTextureResult::AddTextureResult()
    : added(false), x(0), y(0)
{}

TextureAtlasPage::AddTextureResult::operator bool() const {
    return this->added;
}