#pragma once
#include "../ITextRenderer.h"
#include "Geometry/TexVertex.h"

#include <libhelpers/Dx/Dx.h>

namespace GameRenderer {
    class TextRendererDx : public ITextRenderer {
    public:
        TextRendererDx(uint32_t rendererId, DxDevice* dxDev, const std::wstring_view& font, float fontSize, const std::wstring_view& text, RGBA8Color textColor);

        void Render(DxDevice* dxDev);

    private:
        RGBA8Color textColor;
        Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
    };
}
