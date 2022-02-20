#pragma once
#include "../IGameRenderer.h"
#include "GameRendererFactoryDx.h"

#include <libhelpers/Dx/Dx.h>
#include <libhelpers/Dx/Renderer/IOutput.h>

namespace GameRenderer {
    class GameRendererDx : public IGameRenderer {
    public:
        GameRendererDx(DxDevice* dxDev, IOutput* output);

        IGameRendererFactory& GetFactory() override;

        void OperationBegin() override;
        void OperationEnd() override;

    private:
        void DoRenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj) override;
        void DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) override;
        void DoRenderText(const std::shared_ptr<ITextRenderer>& obj) override;

        DxDevice* dxDev = nullptr;
        IOutput* output = nullptr;

        GameRendererFactoryDx factory;
    };
}
