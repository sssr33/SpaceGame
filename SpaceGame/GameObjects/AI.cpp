#include "AI.h"

#include <algorithm>

void AI::StartGame(const StartData& startData, GameRenderer::IGameRendererFactory& factory) {
    this->startData = startData;

    this->playerStartHealth = this->player.GetHealth();
    this->playerColor = RGBA8Color(0, 72, 145);
    this->playerShipSize = 0.2f;

    Math::Vector2 playerStart;

    playerStart.x = (this->GetMainRect().left + this->GetMainRect().right) / 2.f;
    playerStart.y = this->GetMainRect().bottom + (this->playerShipSize / 2.f) + 0.03f;

    this->player.SetPos(playerStart);

    this->playerRectRenderer = factory.MakeRectangleRenderer();
    this->playerRectFillRenderer = factory.MakeRectangleRenderer();

    {
        RGBA8Color color[2] = {};

        color[0].a = 0;
        color[0].r = 255;
        color[0].g = 255;
        color[0].b = 255;

        color[1].a = 255;
        color[1].r = 255;
        color[1].g = 255;
        color[1].b = 255;

        this->gunSmokeTex = factory.MakeTexture2DFromMemory(1, 2, GameRenderer::TexturePixelFormat::BGRA8, &color, sizeof(color));
    }

    this->gun.resize(this->startData.sectors + 1);
}

void AI::Update(float dt) {
    for (auto& g : this->gun) {
        this->Hit(g);
    }
}

void AI::Draw(GameRenderer::IGameRenderer& renderer, float dt) {
    this->DrawPlayer(renderer);

    if (!gun.empty()) {
        constexpr float SmokeLength = 0.5f;
        constexpr float BulletSpeed = 1.f;

        DrawBullets(renderer, gun[0], SmokeLength, BulletSpeed, dt, RGBA8Color(255, 255, 255));

        for (size_t i = 1; i < gun.size(); ++i) {
            DrawBullets(renderer, gun[i], SmokeLength, -BulletSpeed, dt, RGBA8Color(255, 0, 0));
        }
    }
}

void AI::PlayerSetPos(float x) {
    if ((x > this->GetMainRect().left + this->playerShipSize / 2.f)
        && (x < this->GetMainRect().right - this->playerShipSize / 2.f)
        )
    {
        this->player.SetPosX(x);
    }
}

void AI::PlayerGunShot(GameRenderer::IGameRendererFactory& factory) {
    if (gun.empty()) {
        return;
    }

    auto pos = this->player.GetPos();

    pos.y += this->playerShipSize / 2.f;

    gun[0].push_front(Bullet(pos, factory, this->gunSmokeTex));
}

void AI::DrawPlayer(GameRenderer::IGameRenderer& renderer) {
    auto playerBox = this->GetPlayerShipBox();

    GameRenderer::HollowRectangleGeometryParams geom;

    geom.color = this->playerColor;
    geom.thickness = 0.001f;
    geom.width = playerBox.right - playerBox.left;
    geom.height = playerBox.top - playerBox.bottom;

    auto transform = this->playerRectRenderer->GetRectangleTransform();

    transform.position.x = (playerBox.right + playerBox.left) / 2.f;
    transform.position.y = (playerBox.bottom + playerBox.top) / 2.f;

    GameRenderer::FilledRectangleGeometryParams fillGeom;

    fillGeom.color = RGBA8Color(0, 0, 0);
    fillGeom.width = geom.width;
    fillGeom.height = geom.height;

    this->playerRectRenderer->SetGeometryParams(geom);
    this->playerRectRenderer->SetRectangleTransform(transform);

    this->playerRectFillRenderer->SetGeometryParams(fillGeom);
    this->playerRectFillRenderer->SetRectangleTransform(transform);

    renderer.RenderRectangle(this->playerRectFillRenderer);
    renderer.RenderRectangle(this->playerRectRenderer);
}

void AI::DrawPlayerFire(GameRenderer::IGameRenderer& renderer) {

}

void AI::DrawBullets(GameRenderer::IGameRenderer& renderer, std::list<Bullet>& g, float smokeLenght, float speed, float dt, RGBA8Color smokeColor) {
    for (auto& bullet : g) {
        Math::Vector2 posChange = bullet.GetPos();

        posChange.y += speed * dt;

        bullet.DrawInPoint(renderer, posChange, smokeLenght, smokeColor);
    }
}

void AI::Hit(std::list<Bullet>& g) {
    const auto& mainRect = this->GetMainRect();

    std::erase_if(g, [&](Bullet& bullet) {
        const auto& checkPos = bullet.GetPos();

        if (!mainRect.IsInside(checkPos)) {
            return true;
        }

        return false;
        });
}

Math::FBox AI::GetPlayerShipBox() const {
    Math::FBox box;
    const auto& playerPos = this->player.GetPos();
    const float halfSize = this->playerShipSize / 2.f;

    box.left = playerPos.x - halfSize;
    box.top = playerPos.y + halfSize;
    box.right = playerPos.x + halfSize;
    box.bottom = playerPos.y - halfSize;

    return box;
}

const Math::FBox& AI::GetMainRect() const {
    return this->startData.mainRect;
}
