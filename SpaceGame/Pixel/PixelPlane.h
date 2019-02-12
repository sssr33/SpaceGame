#pragma once

#include <cstdint>

struct PixelPlane {
    uint32_t pitch;
    void *dest;
};

struct PixelPlaneReadonly {
    uint32_t pitch;
    void const *dest;
};