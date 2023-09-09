#pragma once

#include <cstdint>
#include <array>

struct RGBA8Color {
    template<class T>
    static constexpr T MaxComponentValue = T(255);

    std::array<uint8_t, 4> rgba = { 0, 0, 0, RGBA8Color::MaxComponentValue<uint8_t> };

    constexpr RGBA8Color() = default;

    constexpr RGBA8Color(uint8_t r, uint8_t g, uint8_t b)
        : RGBA8Color(r, g, b, RGBA8Color::MaxComponentValue<uint8_t>)
    {}

    constexpr RGBA8Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : rgba({r, g, b, a})
    {}

    constexpr bool operator==(const RGBA8Color& other) const = default;

    uint32_t GetRGBA8() const;
    void SetRGBA8(uint32_t packed);

    uint32_t GetBGRA8() const;
    void SetBGRA8(uint32_t packed);

    uint8_t r() const;
    void r(uint8_t val);

    uint8_t g() const;
    void g(uint8_t val);

    uint8_t b() const;
    void b(uint8_t val);

    uint8_t a() const;
    void a(uint8_t val);

    float rf() const;
    void rf(float r);

    float gf() const;
    void gf(float g);

    float bf() const;
    void bf(float b);

    float af() const;
    void af(float a);

private:
    static float GetFloatComponent(uint8_t val);
    static uint8_t SetFloatComponent(float val);
};
