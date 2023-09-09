#include "Sector.h"

Sector::Sector(Math::FBox rect, GameRenderer::IGameRendererFactory& factory)
    : rect(rect),
    renderer(factory.MakeRectangleRenderer())
{
    GameRenderer::FilledRectangleGeometryParams geom;

    geom.width = 0.002f;
    geom.height = rect.Height();
    geom.color.r(255);
    geom.color.g(255);
    geom.color.b(255);

    this->renderer->SetGeometryParams(geom);

    auto transform = this->renderer->GetRectangleTransform();

    transform.position.x = this->rect.Left();
    transform.position.y = this->rect.Center().y;

    this->renderer->SetRectangleTransform(transform);
}

void Sector::Draw(GameRenderer::IGameRenderer& renderer) {
    renderer.RenderRectangle(this->renderer);
}

float Sector::Left() const {
    return this->rect.Left();
}

float Sector::Top() const {
    return this->rect.Top();
}

float Sector::Right() const {
    return this->rect.Right();
}

float Sector::Bottom() const {
    return this->rect.Bottom();
}
