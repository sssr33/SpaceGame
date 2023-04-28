#pragma once
#include "Vector.h"

namespace Math {
    struct IBox {
        int left = 0;
        int top = 0;
        int right = 0;
        int bottom = 0;
    };

    class FBox {
    public:
        bool IsInside(const Math::Vector2& v) const noexcept;

        float Width() const noexcept;
        float Height() const noexcept;

        Vector2 Center() const noexcept;
        void Center(const Vector2& pos) noexcept;

        float Left() const noexcept;
        float Top() const noexcept;
        float Right() const noexcept;
        float Bottom() const noexcept;

        static FBox FromSquare(float size) noexcept;
        static FBox FromSquare(float size, const Math::Vector2& center) noexcept;
        static FBox FromRect(float width, float height) noexcept;
        static FBox FromRect(float width, float height, const Math::Vector2& center) noexcept;
        static FBox FromRect(const Math::Vector2& size, const Math::Vector2& center) noexcept;
        static FBox FromLTRB(float left, float top, float right, float bottom) noexcept;

    private:
        float HalfWidth() const noexcept;
        float HalfHeight() const noexcept;

        Math::Vector2 center;
        Math::Vector2 size;
    };
}
