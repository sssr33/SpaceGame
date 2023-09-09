#include "Stars.h"

Stars::Stars(float speed, size_t starCount, const Math::Vector2& gameFieldSize, GameRenderer::IGameRendererFactory& factory)
    : gameFieldSize(gameFieldSize)
    , starSpeed(speed)
    , starSize(0.001f)
{
    this->stars.reserve(starCount);

    for (size_t i = 0; i < starCount; ++i) {
        auto star = factory.MakeRectangleRenderer();

        GameRenderer::FilledRectangleGeometryParams geom;

        geom.width = this->starSize;
        geom.height = geom.width;
        geom.color.r(255);
        geom.color.g(255);
        geom.color.b(255);

        star->SetGeometryParams(geom);
        this->ResetStarPos(*star, false);

        this->stars.push_back(std::move(star));
    }
}

void Stars::Update(float dt) {
    for (const auto& star : this->stars) {
        auto transform = star->GetRectangleTransform();

        transform.position.y -= this->starSpeed * dt;

        if (transform.position.y - this->starSize < (-this->gameFieldSize.y * 0.5f)) {
            this->ResetStarPos(*star, true);
        }
        else {
            star->SetRectangleTransform(transform);
        }
    }
}

void Stars::Draw(GameRenderer::IGameRenderer& renderer) {
    for (const auto& star : this->stars) {
        renderer.RenderRectangle(star);
    }
}

void Stars::ResetStarPos(GameRenderer::IRectangleRenderer& star, bool startFromTop) const {
    float rndX = static_cast<float>(rand() % 1000) / 1000.f;
    auto transform = star.GetRectangleTransform();

    transform.position.x = -(this->gameFieldSize.x * 0.5f) + this->gameFieldSize.x * rndX;

    if (startFromTop) {
        transform.position.y = this->gameFieldSize.y * 0.5f;
    }
    else {
        float rndY = static_cast<float>(rand() % 1000) / 1000.f;
        transform.position.y = -(this->gameFieldSize.y * 0.5f) + this->gameFieldSize.y * rndY;
    }

    star.SetRectangleTransform(transform);
}
