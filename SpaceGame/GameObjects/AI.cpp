#include "AI.h"

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
}

void AI::Update(float dt) {

}

void AI::Draw(GameRenderer::IGameRenderer& renderer) {
    this->DrawPlayer(renderer);
}

void AI::PlayerSetPos(float x) {
    // if ((posX >(mainRect[0].x + player_ship_size / 2)) && (posX < (mainRect[1].x - player_ship_size / 2))) player.set_posX(posX);

    if ((x > this->GetMainRect().left + this->playerShipSize / 2.f)
        && (x < this->GetMainRect().right - this->playerShipSize / 2.f)
        )
    {
        this->player.SetPosX(x);
    }
}

void AI::PlayerGunShot() {

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
