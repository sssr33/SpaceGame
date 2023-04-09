#pragma once
#include "Renderer/GameRenderer/IGameRenderer.h"
#include "Renderer/GameRenderer/IRectangleRenderer.h"

#include <vector>
#include <memory>
#include <libhelpers/Math/Vector.h>

class Stars {
public:
    Stars(float speed, size_t starCount, const Math::Vector2& gameFieldSize, GameRenderer::IGameRendererFactory& factory);

    void Update(float dt);

    void Draw(GameRenderer::IGameRenderer& renderer);

private:
    void ResetStarPos(GameRenderer::IRectangleRenderer& star, bool startFromTop) const;

    Math::Vector2 gameFieldSize;
    float starSpeed = 0.f;
    float starSize = 0.f;
    std::vector<std::shared_ptr<GameRenderer::IRectangleRenderer>> stars;
};
