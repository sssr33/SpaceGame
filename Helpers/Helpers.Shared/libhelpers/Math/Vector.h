#pragma once

namespace Math {
    struct Vector2 {
        float x = 0.f;
        float y = 0.f;

        Vector2 operator-(const Vector2& other) const noexcept;

        float Dot(const Vector2& other) const noexcept;
        float LengthSq() const noexcept;
        float Length() const noexcept;
        void Normalize() noexcept;
        Vector2 Normalized() const noexcept;
        float Angle(const Vector2& other) const noexcept;
    };

    struct Vector3 {
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
    };

    struct Vector4 {
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
        float w = 0.f;

        float Dot(const Vector4& other) const noexcept;
    };
}
