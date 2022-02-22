#include "GameRendererDx.h"
#include "BackgroundBrushRendererDx.h"
#include "RectangleRendererDx.h"
#include "TextRendererDx.h"

#include <libhelpers/HashString.h>

namespace GameRenderer {
    GameRendererDx::GameRendererDx(DxDevice* dxDev, IOutput* output)
        : IGameRenderer(HashString<uint32_t>("GameRendererDx"))
        , dxDev(dxDev)
        , output(output)
        , factory(this->GetRendererId(), dxDev, output)
    {}

    IGameRendererFactory& GameRendererDx::GetFactory() {
        return this->factory;
    }

    void GameRendererDx::OperationBegin() {
        this->dxDev->LockCtx();
    }

    void GameRendererDx::OperationEnd() {
        this->dxDev->UnlockCtx();
    }

    void GameRendererDx::DoRenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj) {
        BackgroundBrushRendererDx& devObj = static_cast<BackgroundBrushRendererDx&>(*obj);
        devObj.Render(this->dxDev);
    }

    void GameRendererDx::DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) {
        RectangleRendererDx& devObj = static_cast<RectangleRendererDx&>(*obj);
        devObj.Render(this->dxDev);
    }

    void GameRendererDx::DoRenderText(const std::shared_ptr<ITextRenderer>& obj) {
        TextRendererDx& devObj = static_cast<TextRendererDx&>(*obj);
        devObj.Render(this->dxDev);
    }
}
