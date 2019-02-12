#pragma once
#include "../Pixel/IPixelSink.h"

#include <libhelpers/Structs.h>

enum class FontPixelFormat {
    Unknown,
    Gray8,
    RGBA32,
    BGRA32
};

class IFontSink : public IPixelSink {
public:
    virtual ~IFontSink() = default;

    virtual void SetSegment(const Structs::Rect<uint32_t> &size) = 0;
};

class IFontSinkBuilder {
public:
    virtual ~IFontSinkBuilder() = default;

    virtual size_t SelectFormat(const FontPixelFormat *fmt, size_t count) = 0;
    virtual void SetSize(const Structs::Size<uint32_t> &size) = 0;

    virtual IFontSink &GetSink() = 0;
};

class IFontRenderer {
public:
    virtual ~IFontRenderer() = default;

    virtual void Render(
        float fontHeight,
        uint32_t charCode,
        IFontSinkBuilder &sinkBuilder) = 0;
};