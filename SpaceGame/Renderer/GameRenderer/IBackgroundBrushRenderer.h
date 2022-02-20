#pragma once
#include "IGameRendererObject.h"

namespace GameRenderer {
    class IBackgroundBrushRenderer : public IGameRendererObject {
    public:
        using IGameRendererObject::IGameRendererObject;
    };
}
