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
        std::shared_ptr<ITextRenderer> MakeTextRenderer() override;

    private:
        DxDevice* dxDev = nullptr;
        IOutput* output = nullptr;
    };
}
