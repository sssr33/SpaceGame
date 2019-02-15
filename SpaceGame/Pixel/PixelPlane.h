#pragma once

#include <cstdint>

struct PixelPlane {
    uint32_t pitch;
    void *memory;
};

struct PixelPlaneReadonly {
    uint32_t pitch;
    void const *src;
};