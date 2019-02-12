#pragma once
#include "PixelPlane.h"

class IPixelSource {
public:
    virtual ~IPixelSource() = 0;

    virtual void Read(
        PixelPlane *plane,
        size_t planeCount) = 0;
};