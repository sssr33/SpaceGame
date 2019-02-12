#pragma once
#include "PixelPlane.h"

class IPixelSink {
public:
    virtual ~IPixelSink() = 0;

    virtual void Write(
        PixelPlaneReadonly *plane,
        size_t planeCount) = 0;
};