#include "Vector.h"

namespace Math {
    float Vector4::Dot(const Vector4& other) const noexcept {
        const float res =
            this->x * other.x +
            this->y * other.y +
            this->z * other.z +
            this->w * other.w;

        return res;
    }
}
