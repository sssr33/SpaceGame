#include "TextureAtlasPage.h"

#include <cassert>
#include <algorithm>

TextureAtlasPage::TextureAtlasPage(const Structs::Size<uint32_t> &byteSize)
    : byteSize(byteSize)
    , curPt(0, 0), nextY(0)
    , memory(this->byteSize.width * this->byteSize.height)
{}

Structs::Size<uint32_t> TextureAtlasPage::GetByteSize() const {
    return this->byteSize;
}

const void *TextureAtlasPage::GetMemory() const {
    return this->memory.data();
}

bool TextureAtlasPage::IsSpaceAvailable(const Structs::Size<uint32_t> &byteSize) const {
    bool curAvailable = false;
    bool nextAvailable = false;

    this->IsSpaceAvailable(byteSize, curAvailable, nextAvailable);

    bool available = curAvailable || nextAvailable;

    return curAvailable;
}

bool TextureAtlasPage::IsSpaceAvailable(const Structs::Size<uint32_t> &byteSize, const Structs::Size<uint32_t> &byteBorderSize) const {
    auto totalSize = Structs::Size<uint32_t>(
        byteSize.width + byteBorderSize.width,
        byteSize.height + byteBorderSize.height);

    return this->IsSpaceAvailable(totalSize);
}

TextureAtlasPage::AddTextureResult TextureAtlasPage::AddTexture(const void *texMemory, const Structs::Size<uint32_t> &byteSize) {
    return this->AddTexture(texMemory, byteSize, 0, 0);
}

TextureAtlasPage::AddTextureResult TextureAtlasPage::AddTexture(const void *texMemory, const Structs::Size<uint32_t> &byteSize, uint32_t stride) {
    return this->AddTexture(texMemory, byteSize, stride, Structs::Size<uint32_t>(0, 0));
}

TextureAtlasPage::AddTextureResult TextureAtlasPage::AddTexture(const void *texMemory, const Structs::Size<uint32_t> &byteSize, const Structs::Size<uint32_t> &byteBorderSize) {
    return this->AddTexture(texMemory, byteSize, byteSize.width, byteBorderSize);
}

TextureAtlasPage::AddTextureResult TextureAtlasPage::AddTexture(const void *texMemory, const Structs::Size<uint32_t> &byteSize, uint32_t stride, const Structs::Size<uint32_t> &byteBorderSize) {
    bool curAvailable = false;
    bool nextAvailable = false;
    AddTextureResult addResult;

    auto totalSize = Structs::Size<uint32_t>(
        byteSize.width + byteBorderSize.width,
        byteSize.height + byteBorderSize.height);

    this->IsSpaceAvailable(totalSize, curAvailable, nextAvailable);

    if (!curAvailable && !nextAvailable) {
        return addResult;
    }
    else if (!curAvailable) {
        assert(nextAvailable);

        this->curPt.x = 0;
        this->curPt.y = this->nextY;
    }

    auto texMemBytes = static_cast<const uint8_t*>(texMemory);

    for (uint32_t y = 0; y < byteSize.height; y++) {
        auto dstLine = &this->memory[this->curPt.x + (this->curPt.y + y) * this->byteSize.width];
        auto srcLine = &texMemBytes[stride * y];

        std::memcpy(dstLine, srcLine, byteSize.width);
    }

    addResult.added = true;
    addResult.leftTop = this->curPt;

    this->curPt.x += byteSize.width + byteBorderSize.width;
    this->nextY = (std::max)(this->nextY, this->curPt.y + byteSize.height + byteBorderSize.height);

    return addResult;
}

void TextureAtlasPage::IsSpaceAvailable(const Structs::Size<uint32_t> &byteSize, bool &curAvailable, bool &nextAvailable) const {
    auto curFreeWidth = this->curPt.x - this->byteSize.width;
    auto curFreeHeight = this->curPt.y - this->byteSize.height;
    auto nextFreeWidth = this->byteSize.width;
    auto nextFreeHeight = this->nextY - this->byteSize.height;

    bool curWidthAvailable = curFreeWidth >= byteSize.width;
    bool curHeightAvailable = curFreeHeight >= byteSize.height;
    bool nextWidthAvailable = nextFreeWidth >= byteSize.width;
    bool nextHeightAvailable = nextFreeHeight >= byteSize.height;

    curAvailable = curWidthAvailable && curHeightAvailable;
    nextAvailable = nextWidthAvailable && nextHeightAvailable;
}




TextureAtlasPage::AddTextureResult::AddTextureResult()
    : added(false), leftTop(0, 0)
{}

TextureAtlasPage::AddTextureResult::operator bool() const {
    return this->added;
}