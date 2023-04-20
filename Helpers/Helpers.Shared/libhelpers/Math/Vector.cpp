#include "Vector.h"

#include <cmath>

namespace Math {
    Vector2 Vector2::operator-(const Vector2& other) const noexcept {
        Vector2 res;

        res.x = this->x - other.x;
        res.y = this->y - other.y;

        return res;
    }

    float Vector2::Dot(const Vector2& other) const noexcept {
        const float res =
            this->x * other.x +
            this->y * other.y;

        return res;
    }

    float Vector2::LengthSq() const noexcept {
        const float res = this->Dot(*this);
        return res;
    }

    float Vector2::Length() const noexcept {
        const float res = std::sqrt(this->LengthSq());
        return res;
    }

    void Vector2::Normalize() noexcept {
        const float lengthInv = 1.f / this->Length();

        this->x *= lengthInv;
        this->y *= lengthInv;
    }

    Vector2 Vector2::Normalized() const noexcept {
        auto res = *this;
        res.Normalize();
        return res;
    }

    float Vector2::Angle(const Vector2& other) const noexcept {
        // https://stackoverflow.com/questions/14066933/direct-way-of-computing-the-clockwise-angle-between-two-vectors

        const float dot = this->Dot(other);
        const float determinant = this->x * other.y - this->y * other.x;

        const float angle = std::atan2(determinant, dot);

        return angle;
    }

    float Vector4::Dot(const Vector4& other) const noexcept {
        const float res =
            this->x * other.x +
            this->y * other.y +
            this->z * other.z +
            this->w * other.w;

        return res;
    }
}
