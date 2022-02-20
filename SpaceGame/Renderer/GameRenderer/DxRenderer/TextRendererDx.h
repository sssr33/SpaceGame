#pragma once
#include "../ITextRenderer.h"

namespace GameRenderer {
    class TextRendererDx : public ITextRenderer {
    public:
        TextRendererDx(uint32_t rendererId);
    };
}
