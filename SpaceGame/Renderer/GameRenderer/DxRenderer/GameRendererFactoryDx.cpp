#include "GameRendererFactoryDx.h"

#include "BackgroundBrushRendererDx.h"
#include "RectangleRendererDx.h"

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

    std::shared_ptr<ITextRenderer> GameRendererFactoryDx::MakeTextRenderer() {
        return nullptr;
    }
}
