#include "Box.h"

namespace Math {
    bool FBox::IsInside(const Math::Vector2& v) const noexcept {
        bool inside = v.x >= this->left && v.x <= this->right
            && v.y >= this->bottom && v.y <= this->top;

        return inside;
    }
}
