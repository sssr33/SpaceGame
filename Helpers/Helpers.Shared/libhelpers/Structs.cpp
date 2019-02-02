#include "Structs.h"

namespace Structs {
    SizeU::SizeU(uint32_t s)
        : width(s), height(s)
    {}

    SizeU::SizeU(uint32_t width, uint32_t height)
        : width(width), height(height)
    {}

    bool SizeU::operator==(const SizeU &other) const {
        bool equWidth = this->width == other.width;
        bool equHeight = this->height == other.height;
        bool equ = equWidth && equHeight;

        return equ;
    }

    bool SizeU::operator!=(const SizeU &other) const {
        bool notEqu = !this->operator==(other);
        return notEqu;
    }




    PointU::PointU(uint32_t x, uint32_t y)
        : x(x), y(y)
    {}

    bool PointU::operator==(const PointU &other) const {
        bool equX = this->x == other.x;
        bool equY = this->y == other.y;
        bool equ = equX && equY;

        return equ;
    }

    bool PointU::operator!=(const PointU &other) const {
        bool notEqu = !this->operator==(other);
        return notEqu;
    }




    RectU::RectU(const PointU &leftTop, const SizeU &size)
        : leftTop(leftTop), size(size)
    {}

    bool RectU::operator==(const RectU &other) const {
        bool equLeftTop = this->leftTop == other.leftTop;
        bool equSize = this->size == other.size;
        bool equ = equLeftTop && equSize;

        return equ;
    }

    bool RectU::operator!=(const RectU &other) const {
        bool notEqu = !this->operator==(other);
        return notEqu;
    }
}