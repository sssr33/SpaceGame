#pragma once
#include "Renderer/GameRenderer/IGameRenderer.h"
#include "Renderer/GameRenderer/IRectangleRenderer.h"

#include <libhelpers/Math/Box.h>

class Sector {
public:
    Sector(Math::FBox rect, GameRenderer::IGameRendererFactory& factory);

    void Draw(GameRenderer::IGameRenderer& renderer);

    float Left() const;
    float Top() const;
    float Right() const;
    float Bottom() const;

private:
    Math::FBox rect;
    std::shared_ptr<GameRenderer::IRectangleRenderer> renderer;
};
