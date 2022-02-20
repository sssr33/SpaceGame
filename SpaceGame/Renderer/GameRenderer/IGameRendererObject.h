#pragma once

#include "GameRendererId.h"

namespace GameRenderer {
    class IGameRendererObject : public GameRendererId {
    public:
        using GameRendererId::GameRendererId;

        virtual ~IGameRendererObject() = default;
    };
}
