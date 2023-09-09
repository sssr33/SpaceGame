#include "RGBA8Color.h"

#include <algorithm>
#include <bit>

uint32_t RGBA8Color::GetRGBA8() const {
    return std::bit_cast<uint32_t>(this->rgba);
}

void RGBA8Color::SetRGBA8(uint32_t packed) {
    this->rgba = std::bit_cast<decltype(this->rgba)>(packed);
}

uint32_t RGBA8Color::GetBGRA8() const {
    std::array<uint8_t, 4> bgra = {
            this->b(), this->g(), this->r(), this->a()
    };

    return std::bit_cast<uint32_t>(bgra);
}

void RGBA8Color::SetBGRA8(uint32_t packed) {
    auto bgra = std::bit_cast<std::array<uint8_t, 4>>(packed);

    this->r(bgra[2]);
    this->g(bgra[1]);
    this->b(bgra[0]);
    this->a(bgra[3]);
}

uint8_t RGBA8Color::r() const {
    return this->rgba[0];
}

void RGBA8Color::r(uint8_t val) {
    this->rgba[0] = val;
}

uint8_t RGBA8Color::g() const {
    return this->rgba[1];
}

void RGBA8Color::g(uint8_t val) {
    this->rgba[1] = val;
}

uint8_t RGBA8Color::b() const {
    return this->rgba[2];
}

void RGBA8Color::b(uint8_t val) {
    this->rgba[2] = val;
}

uint8_t RGBA8Color::a() const {
    return this->rgba[3];
}

void RGBA8Color::a(uint8_t val) {
    this->rgba[3] = val;
}

float RGBA8Color::rf() const {
    return RGBA8Color::GetFloatComponent(this->r());
}

void RGBA8Color::rf(float r) {
    this->r(RGBA8Color::SetFloatComponent(r));
}

float RGBA8Color::gf() const {
    return RGBA8Color::GetFloatComponent(this->g());
}

void RGBA8Color::gf(float g) {
    this->g(RGBA8Color::SetFloatComponent(g));
}

float RGBA8Color::bf() const {
    return RGBA8Color::GetFloatComponent(this->b());
}

void RGBA8Color::bf(float b) {
    this->b(RGBA8Color::SetFloatComponent(b));
}

float RGBA8Color::af() const {
    return RGBA8Color::GetFloatComponent(this->a());
}

void RGBA8Color::af(float a) {
    this->a(RGBA8Color::SetFloatComponent(a));
}

float RGBA8Color::GetFloatComponent(uint8_t val) {
    float res = static_cast<float>(val) / RGBA8Color::MaxComponentValue<float>;
    return res;
}

uint8_t RGBA8Color::SetFloatComponent(float val) {
    uint8_t res = static_cast<uint8_t>(std::clamp(
        val * RGBA8Color::MaxComponentValue<float>,
        0.f,
        RGBA8Color::MaxComponentValue<float>
    ));
    return res;
}
