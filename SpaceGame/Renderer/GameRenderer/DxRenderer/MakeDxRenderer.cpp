#include "MakeDxRenderer.h"

#include "GameRendererDx.h"

namespace GameRenderer {
    std::shared_ptr<IGameRenderer> MakeDxRenderer::operator()(DxDevice* dxDev, IOutput* output) {
        return std::make_shared<GameRendererDx>(dxDev, output);
    }
}
