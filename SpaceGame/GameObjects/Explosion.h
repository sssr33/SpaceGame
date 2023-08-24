#pragma once
#include "Renderer/GameRenderer/IGameRenderer.h"
#include "Renderer/GameRenderer/IRectangleRenderer.h"

#include <libhelpers/Math/Vector.h>

class Explosion {
public:
    Explosion(const Math::Vector2& pos, float finalRadius);

    void Draw(GameRenderer::IGameRenderer& renderer, float dt);

    bool IsActive() const;

private:
    void CheckRenderers(GameRenderer::IGameRendererFactory& factory);

    std::shared_ptr<GameRenderer::IRectangleRenderer> innerPart;
    std::shared_ptr<GameRenderer::IRectangleRenderer> outerPart;
    float innerRadius = 0.f;
    float outerRadius = 0.f;
    float finalRadius = 0.f;
    Math::Vector2 pos;
    bool isActive = true;
};
