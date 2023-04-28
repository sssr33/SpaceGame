#include "Box.h"

#include <cmath>

namespace Math {
    bool FBox::IsInside(const Math::Vector2& v) const noexcept {
        const Math::Vector2 dist = {
            std::abs(this->center.x - v.x),
            std::abs(this->center.y - v.y)
        };

        const bool xInside = dist.x <= this->HalfWidth();
        const bool yInside = dist.y <= this->HalfHeight();

        const bool inside = xInside && yInside;

        return inside;
    }

    float FBox::Width() const noexcept {
        return this->size.x;
    }

    float FBox::Height() const noexcept {
        return this->size.y;
    }

    Vector2 FBox::Center() const noexcept {
        return this->center;
    }

    void FBox::Center(const Vector2& pos) noexcept {
        this->center = pos;
    }

    float FBox::Left() const noexcept {
        const float v = this->Center().x - this->HalfWidth();
        return v;
    }

    float FBox::Top() const noexcept {
        const float v = this->Center().y + this->HalfHeight();
        return v;
    }

    float FBox::Right() const noexcept {
        const float v = this->Center().x + this->HalfWidth();
        return v;
    }

    float FBox::Bottom() const noexcept {
        const float v = this->Center().y - this->HalfHeight();
        return v;
    }

    FBox FBox::FromSquare(float size) noexcept {
        return FBox::FromSquare(size, {});
    }

    FBox FBox::FromSquare(float size, const Math::Vector2& center) noexcept {
        return FBox::FromRect(size, size, center);
    }

    FBox FBox::FromRect(float width, float height) noexcept {
        return FBox::FromRect(width, height, {});
    }

    FBox FBox::FromRect(float width, float height, const Math::Vector2& center) noexcept {
        return FBox::FromRect({ width, height }, center);
    }

    FBox FBox::FromRect(const Math::Vector2& size, const Math::Vector2& center) noexcept {
        FBox box;

        box.center = center;
        box.size = size;

        return box;
    }

    FBox FBox::FromLTRB(float left, float top, float right, float bottom) noexcept {
        const float centerX = (left + right) / 2.f;
        const float centerY = (top + bottom) / 2.f;
        const float width = std::abs(right - left);
        const float height = std::abs(top - bottom);

        return FBox::FromRect({ width, height }, { centerX, centerY });
    }

    float FBox::HalfWidth() const noexcept {
        return this->Width() / 2.f;
    }

    float FBox::HalfHeight() const noexcept {
        return this->Height() / 2.f;
    }
}
