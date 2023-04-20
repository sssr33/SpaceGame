#pragma once
#include "Renderer/GameRenderer/IGameRenderer.h"
#include "Renderer/GameRenderer/IRectangleRenderer.h"

#include <libhelpers/Math/Vector.h>

// BULLET
class Bullet {
public:
    Bullet(const Math::Vector2& startPos, GameRenderer::IGameRendererFactory& factory, std::shared_ptr<GameRenderer::ITexture2D> bulletSmokeTex);

    const Math::Vector2& GetPos() const;

    void DrawInPoint(GameRenderer::IGameRenderer& renderer, const Math::Vector2& pos, float smokeLenght, RGBA8Color smokeColor);

private:
    void DrawModel(GameRenderer::IGameRenderer& renderer);
    void DrawSmoke(GameRenderer::IGameRenderer& renderer, float smokeLenght, RGBA8Color smokeColor, const Math::Vector2& flyDir);

    static const float ModelWidth;
    static const float ModelHeight;

    Math::Vector2 startBulletPos; // start_bullet_pos
    Math::Vector2 posOfBullet; // pos_of_bullet

    std::shared_ptr<GameRenderer::ITexture2D> bulletSmokeTex;
    std::shared_ptr<GameRenderer::IRectangleRenderer> model;
    std::shared_ptr<GameRenderer::IRectangleRenderer> smoke;
};
