#include "GameRendererFactoryDx.h"

#include "BackgroundBrushRendererDx.h"
#include "RectangleRendererDx.h"
#include "TextRendererDx.h"
#include "Texture2DDx.h"

namespace GameRenderer {
    GameRendererFactoryDx::GameRendererFactoryDx(uint32_t rendererId, DxDevice* dxDev, IOutput* output)
        : IGameRendererFactory(rendererId)
        , dxDev(dxDev)
        , output(output)
    {}

    std::shared_ptr<IBackgroundBrushRenderer> GameRendererFactoryDx::MakeBackgroundBrushRenderer(const std::wstring& texName) {
        return std::make_shared<BackgroundBrushRendererDx>(this->GetRendererId(), this->dxDev, texName);
    }

    std::shared_ptr<IRectangleRenderer> GameRendererFactoryDx::MakeRectangleRenderer() {
        return std::make_shared<RectangleRendererDx>(this->GetRendererId(), this->dxDev);
    }

    std::shared_ptr<ITextRenderer> GameRendererFactoryDx::MakeTextRenderer(const std::wstring_view& font, float fontSize, const std::wstring_view& text, RGBA8Color textColor) {
        return std::make_shared<TextRendererDx>(this->GetRendererId(), this->dxDev, font, fontSize, text, textColor);
    }

    std::shared_ptr<ITexture2D> GameRendererFactoryDx::MakeTexture2DFromMemory(uint32_t width, uint32_t height, TexturePixelFormat pixFormat, const void* mem, size_t memSize) {
        return std::make_shared<Texture2DDx>(this->GetRendererId(), this->dxDev, width, height, GameRendererFactoryDx::ToDXGI_FORMAT(pixFormat), mem, memSize);
    }

    DXGI_FORMAT GameRendererFactoryDx::ToDXGI_FORMAT(TexturePixelFormat pixFormat) {
        switch (pixFormat) {
        case GameRenderer::TexturePixelFormat::BGRA8:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        }

        return DXGI_FORMAT_UNKNOWN;
    }
}
