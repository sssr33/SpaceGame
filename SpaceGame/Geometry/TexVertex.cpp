#include "TexVertex.h"

RGBA8Color::RGBA8Color(uint8_t r, uint8_t g, uint8_t b)
    : RGBA8Color(r, g, b, 255)
{}

RGBA8Color::RGBA8Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r(r), g(g), b(b), a(a)
{}
