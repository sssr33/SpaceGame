#pragma once
#include "../ITextRenderer.h"

#include <libhelpers\Dx\Dx.h>

namespace GameRenderer {
    class TextRendererDx : public ITextRenderer {
    public:
        TextRendererDx(uint32_t rendererId, DxDevice* dxDev, const std::wstring_view& font, float fontSize, const std::wstring_view& text);

        void Render(DxDevice* dxDev);

    private:
        Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
    };
}
