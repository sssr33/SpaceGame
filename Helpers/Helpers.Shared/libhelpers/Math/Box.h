#pragma once
#include "Vector.h"

namespace Math {
    struct IBox {
        int left = 0;
        int top = 0;
        int right = 0;
        int bottom = 0;
    };

    struct FBox {
        float left = 0.f;
        float top = 0.f;
        float right = 0.f;
        float bottom = 0.f;

        bool IsInside(const Math::Vector2& v) const noexcept;
    };
}
