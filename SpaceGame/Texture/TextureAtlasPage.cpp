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

std::vector<Structs::Size<uint32_t>> TextureAtlasPage::GetAvailableRects() const {
    std::vector<Structs::Size<uint32_t>> rects;
    assert(false); // need to impl

    return rects;
}

bool TextureAtlasPage::IsSpaceAvailable(const Structs::Size<uint32_t> &byteSize) const {
    auto spaceAvailRes = this->IsSpaceAvailableInternal(byteSize);
    bool available = spaceAvailRes.curAvailable || spaceAvailRes.nextAvailable;

    return available;
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
    AddTextureResult addResult;

    auto spaceAvailRes = this->IsSpaceAvailableInternal(byteSize);

    if (!spaceAvailRes.curAvailable && !spaceAvailRes.nextAvailable) {
        return addResult;
    }
    else if (!spaceAvailRes.curAvailable) {
        assert(spaceAvailRes.nextAvailable);

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

    // only now take border into account so it will affect position of next texture
    this->curPt.x += byteSize.width + byteBorderSize.width;
    this->nextY = (std::max)(this->nextY, this->curPt.y + byteSize.height + byteBorderSize.height);

    return addResult;
}

TextureAtlasPage::IsSpaceAvailableRes TextureAtlasPage::IsSpaceAvailableInternal(const Structs::Size<uint32_t> &byteSize) const {
    bool curWidthAvailable = TextureAtlasPage::IsSizeAvailable(this->curPt.x, this->byteSize.width, byteSize.width);
    bool curHeightAvailable = TextureAtlasPage::IsSizeAvailable(this->curPt.y, this->byteSize.height, byteSize.height);
    bool nextWidthAvailable = TextureAtlasPage::IsSizeAvailable(0, this->byteSize.width, byteSize.width);
    bool nextHeightAvailable = TextureAtlasPage::IsSizeAvailable(this->nextY, this->byteSize.height, byteSize.height);

    IsSpaceAvailableRes res;

    res.curAvailable = curWidthAvailable && curHeightAvailable;
    res.nextAvailable = nextWidthAvailable && nextHeightAvailable;

    return res;
}

bool TextureAtlasPage::IsSizeAvailable(uint32_t point, uint32_t size, uint32_t desiredSize) {
    if (point < size) {
        auto freeSize = size - point;
        bool available = freeSize >= desiredSize;
        return available;
    }

    return false;
}




TextureAtlasPage::AddTextureResult::AddTextureResult()
    : added(false), leftTop(0, 0)
{}

TextureAtlasPage::AddTextureResult::operator bool() const {
    return this->added;
}