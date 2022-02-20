#pragma once

#include "../IGameRenderer.h"

#include <memory>
#include <libhelpers/Dx/Dx.h>
#include <libhelpers/Dx/Renderer/IOutput.h>

namespace GameRenderer {
    struct MakeDxRenderer {
        std::shared_ptr<IGameRenderer> operator()(DxDevice* dxDev, IOutput* output);
    };
}
