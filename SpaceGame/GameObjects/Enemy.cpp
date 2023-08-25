#include "Enemy.h"

Enemy::Enemy(GameRenderer::IGameRendererFactory& factory)
    : direction(10)
{
    this->direction.SetActive(0);
    this->model = Math::FBox::FromSquare(0.2f);

    this->modelRenderer = factory.MakeRectangleRenderer();
    this->modelFillRenderer = factory.MakeRectangleRenderer();
    this->modelCenterDotRenderer = factory.MakeRectangleRenderer();

    // TODO add outline
    // pen - outline - brush fill
    // pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(0, 23, 40)));
    // brush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(0, 35, 62)));
    // pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(0, 23, 40)));
    // brush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(0, 49, 85)));
    this->portalInnerFillRenderer = factory.MakeRectangleRenderer();
    this->portalOuterFillRenderer = factory.MakeRectangleRenderer();

    GameRenderer::FilledRectangleGeometryParams portalInnerGeom;

    portalInnerGeom.color = RGBA8Color(0, 35, 62);
    portalInnerGeom.width = 1.f;
    portalInnerGeom.height = 1.f;
    portalInnerGeom.roundness = 1.f;

    auto portalOuterGeom = portalInnerGeom;

    portalOuterGeom.color = RGBA8Color(0, 49, 85);

    this->portalInnerFillRenderer->SetGeometryParams(portalInnerGeom);
    this->portalOuterFillRenderer->SetGeometryParams(portalOuterGeom);
}

void Enemy::Draw(GameRenderer::IGameRenderer& renderer) {
    if (this->status && !this->unset) {
        GameRenderer::HollowRectangleGeometryParams geom;

        geom.color = RGBA8Color(255, 255, 255);
        geom.thickness = 0.001f;
        geom.width = this->model.Width();
        geom.height = this->model.Height();

        auto transform = this->modelRenderer->GetRectangleTransform();

        transform.position.x = this->model.Center().x;
        transform.position.y = this->model.Center().y;

        GameRenderer::FilledRectangleGeometryParams fillGeom;

        fillGeom.color = RGBA8Color(0, 0, 0);
        fillGeom.width = geom.width;
        fillGeom.height = geom.height;

        GameRenderer::FilledRectangleGeometryParams fillGeomDot;

        fillGeomDot.color = RGBA8Color(255, 0, 0);
        fillGeomDot.width = geom.width * 0.001f;
        fillGeomDot.height = geom.width * 0.001f;

        this->modelRenderer->SetGeometryParams(geom);
        this->modelRenderer->SetRectangleTransform(transform);

        this->modelFillRenderer->SetGeometryParams(fillGeom);
        this->modelFillRenderer->SetRectangleTransform(transform);

        this->modelCenterDotRenderer->SetGeometryParams(fillGeomDot);
        this->modelCenterDotRenderer->SetRectangleTransform(transform);

        renderer.RenderRectangle(this->modelFillRenderer);
        renderer.RenderRectangle(this->modelCenterDotRenderer);
        renderer.RenderRectangle(this->modelRenderer);
    }
}

void Enemy::DrawRespawnPortal(GameRenderer::IGameRenderer& renderer, float dt) {
    if (!this->status && !this->unset) {
        if (!this->portalStarted) {
            this->portalStarted = true;

            this->respawnTime = 1 + rand() % 5;
            this->respawnTimer = 0.f;
            this->portalPulse = 0.f;
            this->portalPulseCount = 0;
        }

        this->respawnTimer += dt;

        auto innerTransform = this->portalOuterFillRenderer->GetRectangleTransform();
        innerTransform.position.x = this->model.Center().x;
        innerTransform.position.y = this->model.Center().y;
        innerTransform.scale.x = this->model.Width();
        innerTransform.scale.y = 0.07f;
        this->portalOuterFillRenderer->SetRectangleTransform(innerTransform);

        auto outerTransform = this->portalInnerFillRenderer->GetRectangleTransform();
        outerTransform.position = innerTransform.position;
        outerTransform.scale = innerTransform.scale;
        outerTransform.scale.x -= this->portalPulse;
        outerTransform.scale.y -= this->portalPulse;
        this->portalInnerFillRenderer->SetRectangleTransform(outerTransform);

        renderer.RenderRectangle(this->portalOuterFillRenderer);
        renderer.RenderRectangle(this->portalInnerFillRenderer);

        if (this->respawnTimer >= this->respawnTime) {
            this->portalPulse += 0.01f;
            ++this->portalPulseCount;
            this->respawnTimer = 0.f;
        }
        if (this->portalPulseCount > 10) {
            this->respawned = true;
        }
    }
}

void Enemy::SetModelPos(const Math::Vector2& pos) {
    this->model.Center(pos);
}

void Enemy::AnimTest(float x, float lim1, float lim2, float dt) {
    if (this->status && !this->unset) {
        const float normalMove = 0.4f * dt;
        const float smallMove = 0.2f * dt;
        const float edge = 0.006f;

        auto center = this->model.Center();
        const float left = this->model.Left();
        const float right = this->model.Right();

        if (x > lim1 && x < lim2) {
            this->targetDetect = true;
            if (x > center.x && (left > lim1) && (right < lim2))
            {
                center.x += normalMove;
            }
            else if (x < center.x && (left > lim1) && (right < lim2))
            {
                center.x -= normalMove;
            }
            else if (x < center.x && (left > lim1) && (right <= lim2 + edge))
            {
                center.x -= normalMove;
            }
            else if (x > center.x && (left >= lim1 - edge) && (right < lim2))
            {
                center.x += normalMove;
            }
        }
        else if (x < lim1 || x > lim2) {
            this->targetDetect = false;
            if (this->direction.IsActive(0) && (left > lim1) && (right < lim2))
            {
                center.x += smallMove;
            }
            else if (this->direction.IsActive(0) && (left > lim1) && (right <= lim2 + edge))
            {
                this->direction.SetActive(1);
                center.x -= smallMove;
            }
            else if (this->direction.IsActive(1) && (left > lim1) && (right < lim2))
            {
                center.x -= smallMove;
            }
            else if (this->direction.IsActive(1) && (left >= lim1 - edge) && (right < lim2))
            {
                this->direction.SetActive(0);
                center.x += smallMove;
            }
        }

        this->model.Center(center);
    }
}

Math::Vector2 Enemy::GetGunPos() {
    Math::Vector2 pos;

    pos.x = this->model.Center().x;
    pos.y = this->model.Bottom() - 0.001f;

    return pos;
}

bool Enemy::GetStatus() const {
    return this->status;
}

bool Enemy::GetUnset() const {
    return this->unset;
}

bool Enemy::GetNeedExplosion() const {
    return this->needExplosion;
}

void Enemy::SetNeedExplosion(bool need) {
    this->needExplosion = need;
}

Math::FBox Enemy::GetEnemyRect() const {
    return this->model;
}

void Enemy::Damage(float damage) {
    if (this->health > 0.f) {
        this->health -= damage;
    }

    if (this->health <= 0.f) {
        this->status = false;
    }
}

bool Enemy::GetDirection() {
    // fix, refactor ??

    bool index = false;
    this->direction.IsActive(0);
    if (index) return index;
    else {
        this->direction.IsActive(1);
        return index;
    }
}

void Enemy::SetDirection(size_t direct) {
    this->direction.SetActive(direct);
}

Math::Vector2 Enemy::GetCenter() {
    return this->model.Center();
}

void Enemy::Respawn(const Math::Vector2& pos, size_t direct) {
    this->health = 100.f;
    this->respawned = false;
    this->status = true;

    this->SetModelPos(pos);
    this->direction.SetActive(direct);

    this->needExplosion = true;
    this->portalStarted = false;
}




Enemy::Switch::Switch(size_t size) {
    this->values.resize(size, false);
}

void Enemy::Switch::SetActive(size_t index) {
    if (index >= this->values.size()) {
        return;
    }

    for (size_t i = 0; i < this->values.size(); i++) {
        this->values[i] = i == index;
    }
}

bool Enemy::Switch::IsActive(size_t index) const {
    if (index < this->values.size()) {
        return this->values[index];
    }

    return false;
}
