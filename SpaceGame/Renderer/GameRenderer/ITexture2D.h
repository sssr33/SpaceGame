#pragma once
#include "IGameRendererObject.h"
#include "TexturePixelFormat.h"

namespace GameRenderer {
    class ITexture2D : public IGameRendererObject {
    public:
        using IGameRendererObject::IGameRendererObject;
    };
}
