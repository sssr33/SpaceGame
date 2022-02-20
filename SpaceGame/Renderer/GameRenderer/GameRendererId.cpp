#include "GameRendererId.h"

namespace GameRenderer {
    GameRendererId::GameRendererId(uint32_t rendererId)
        : rendererId(rendererId)
    {}

    bool GameRendererId::IsSameRenderer(const GameRendererId & obj) const {
        return this->rendererId == obj.rendererId;
    }

    uint32_t GameRendererId::GetRendererId() const {
        return rendererId;
    }
}
