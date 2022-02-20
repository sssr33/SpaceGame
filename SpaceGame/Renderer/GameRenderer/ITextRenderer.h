#pragma once
#include "IGameRendererObject.h"

namespace GameRenderer {
    class ITextRenderer : public IGameRendererObject {
    public:
        using IGameRendererObject::IGameRendererObject;
    };
}
