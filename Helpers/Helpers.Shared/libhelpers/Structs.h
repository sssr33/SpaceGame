#pragma once

#include <cstdint>

namespace Structs {
    struct SizeU {
        uint32_t width;
        uint32_t height;

        SizeU() = default;
        SizeU(uint32_t s);
        SizeU(uint32_t width, uint32_t height);

        bool operator==(const SizeU &other) const;
        bool operator!=(const SizeU &other) const;
    };

    struct PointU {
        uint32_t x;
        uint32_t y;

        PointU() = default;
        PointU(uint32_t x, uint32_t y);

        bool operator==(const PointU &other) const;
        bool operator!=(const PointU &other) const;
    };

    struct RectU {
        PointU leftTop;
        SizeU size;

        RectU() = default;
        RectU(const PointU &leftTop, const SizeU &size);

        bool operator==(const RectU &other) const;
        bool operator!=(const RectU &other) const;
    };
}