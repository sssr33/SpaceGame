#pragma once

#include <cstdint>

namespace GameRenderer {
    class GameRendererId {
    public:
        GameRendererId(uint32_t rendererId);

        virtual ~GameRendererId() = default;

        bool IsSameRenderer(const GameRendererId& obj) const;
        uint32_t GetRendererId() const;

    private:
        uint32_t rendererId = 0;
    };
}
