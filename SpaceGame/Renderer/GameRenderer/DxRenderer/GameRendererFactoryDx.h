#pragma once
#include "../IGameRendererFactory.h"

#include <libhelpers/Dx/Dx.h>
#include <libhelpers/Dx/Renderer/IOutput.h>

namespace GameRenderer {
    class GameRendererFactoryDx : public IGameRendererFactory {
    public:
        GameRendererFactoryDx(uint32_t rendererId, DxDevice* dxDev, IOutput* output);

        std::shared_ptr<IBackgroundBrushRenderer> MakeBackgroundBrushRenderer(const std::wstring& texName) override;
        std::shared_ptr<IRectangleRenderer> MakeRectangleRenderer() override;
        std::shared_ptr<ITextRenderer> MakeTextRenderer(const std::wstring_view& font, float fontSize, const std::wstring_view& text, RGBA8Color textColor) override;
        std::shared_ptr<ITexture2D> MakeTexture2DFromMemory(uint32_t width, uint32_t height, TexturePixelFormat pixFormat, const void* mem, size_t memSize) override;

    private:
        static DXGI_FORMAT ToDXGI_FORMAT(TexturePixelFormat pixFormat);

        DxDevice* dxDev = nullptr;
        IOutput* output = nullptr;
    };
}
