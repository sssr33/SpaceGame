#include "GameRendererFactoryDx.h"

#include "BackgroundBrushRendererDx.h"
#include "RectangleRendererDx.h"
#include "TextRendererDx.h"

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

    std::shared_ptr<ITextRenderer> GameRendererFactoryDx::MakeTextRenderer(const std::wstring_view& font, float fontSize, const std::wstring_view& text) {
        return std::make_shared<TextRendererDx>(this->GetRendererId(), this->dxDev, font, fontSize, text);
    }
}
