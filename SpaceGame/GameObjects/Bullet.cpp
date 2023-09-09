#include "Bullet.h"

const float Bullet::ModelWidth = 0.001f;
const float Bullet::ModelHeight = 0.005f;

Bullet::Bullet(const Math::Vector2& startPos, GameRenderer::IGameRendererFactory& factory, std::shared_ptr<GameRenderer::ITexture2D> bulletSmokeTex)
    : startBulletPos(startPos)
    , posOfBullet(startPos)
    , bulletSmokeTex(std::move(bulletSmokeTex))
{
    {
        this->model = factory.MakeRectangleRenderer();

        GameRenderer::FilledRectangleGeometryParams geom;

        geom.width = Bullet::ModelWidth;
        geom.height = Bullet::ModelHeight;
        geom.color.r(255);
        geom.color.g(0);
        geom.color.b(0);

        this->model->SetGeometryParams(geom);
    }

    {
        this->smoke = factory.MakeRectangleRenderer();
    }
}

const Math::Vector2& Bullet::GetPos() const {
    return this->posOfBullet;
}

void Bullet::DrawInPoint(GameRenderer::IGameRenderer& renderer, const Math::Vector2& pos, float smokeLenght, RGBA8Color smokeColor) {
    this->posOfBullet = pos;

    const auto flyVec = this->posOfBullet - this->startBulletPos;
    const float maxSmokeLength = flyVec.Length();
    const float curSmokeLength = (std::min)(maxSmokeLength, smokeLenght);

    this->DrawSmoke(renderer, curSmokeLength, smokeColor, flyVec.Normalized());
    this->DrawModel(renderer);
}

void Bullet::DrawModel(GameRenderer::IGameRenderer& renderer) {
    auto transform = this->model->GetRectangleTransform();

    transform.position = { this->posOfBullet.x, this->posOfBullet.y, 1.f };

    this->model->SetRectangleTransform(transform);

    renderer.RenderRectangle(this->model);
}

void Bullet::DrawSmoke(GameRenderer::IGameRenderer& renderer, float smokeLenght, RGBA8Color smokeColor, const Math::Vector2& flyDir) {
    GameRenderer::FilledRectangleGeometryParams geom;

    geom.width = Bullet::ModelWidth;
    geom.height = smokeLenght;
    geom.color = smokeColor;

    auto transform = this->smoke->GetRectangleTransform();

    transform.position = { this->posOfBullet.x, this->posOfBullet.y, 1.f };
    transform.position.y -= flyDir.y * (Bullet::ModelHeight / 2.f + smokeLenght / 2.f);

    transform.rotation.z = flyDir.Angle({ 0.f, -1.f });

    this->smoke->SetGeometryParams(geom);
    this->smoke->SetRectangleTransform(transform);

    renderer.RenderRectangle(this->smoke, this->bulletSmokeTex);
}
