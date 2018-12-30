#pragma once

#include <cstdint>

struct IFontAtlasBitmapBytes {
    const void *data;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bitmapId;
    uint32_t changeId;

    uint32_t symbolLeft;
    uint32_t symbolTop;
    uint32_t symbolWidth;
    uint32_t symbolHeight;
};

/*
Implementation must keep whole IFontAtlasBitmapBytes alive and immutable.
*/
class IFontAtlasBitmapLock {
public:
    virtual ~IFontAtlasBitmapLock() = default;

    virtual IFontAtlasBitmapBytes GetBytes() = 0;
};