#include "Explosion.h"

Explosion::Explosion(const Math::Vector2& pos, float finalRadius)
    : finalRadius(finalRadius)
    , pos(pos)
{}

void Explosion::Draw(GameRenderer::IGameRenderer& renderer, float dt) {
    if (!this->isActive) {
        return;
    }

    this->CheckRenderers(renderer.GetFactory());

    constexpr float Speed = 0.1f;
    constexpr float InnerExplosionSpeed = 0.45f * Speed;
    constexpr float OuterExplosionSpeed = 1.5f * Speed;

    this->innerRadius += InnerExplosionSpeed * dt;
    this->outerRadius += OuterExplosionSpeed * dt;

    auto innerTransform = this->innerPart->GetRectangleTransform();
    auto outerTransform = this->outerPart->GetRectangleTransform();

    innerTransform.scale = { this->innerRadius, this->innerRadius };
    outerTransform.scale = { this->outerRadius, this->outerRadius };

    this->innerPart->SetRectangleTransform(innerTransform);
    this->outerPart->SetRectangleTransform(outerTransform);

    renderer.RenderRectangle(this->outerPart);
    renderer.RenderRectangle(this->innerPart);

    this->isActive =
        this->innerRadius < this->finalRadius
        && this->outerRadius < this->finalRadius;
}

bool Explosion::IsActive() const {
    return this->isActive;
}

void Explosion::CheckRenderers(GameRenderer::IGameRendererFactory& factory) {
    if (this->innerPart && this->outerPart) {
        return;
    }

    this->innerPart = factory.MakeRectangleRenderer();
    this->outerPart = factory.MakeRectangleRenderer();

    GameRenderer::FilledRectangleGeometryParams innerGeom;

    innerGeom.width = 1.f;
    innerGeom.height = 1.f;
    innerGeom.color = RGBA8Color(255, 56, 56);

    auto outerGeom = innerGeom;

    outerGeom.color = RGBA8Color(255, 56, 6);

    this->innerPart->SetGeometryParams(innerGeom);
    this->outerPart->SetGeometryParams(outerGeom);

    auto innerTransform = this->innerPart->GetRectangleTransform();
    auto outerTransform = this->outerPart->GetRectangleTransform();

    innerTransform.position = { this->pos.x, this->pos.y, 1.f };
    outerTransform.position = { this->pos.x, this->pos.y, 1.f };

    this->innerPart->SetRectangleTransform(innerTransform);
    this->outerPart->SetRectangleTransform(outerTransform);
}
