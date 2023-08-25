#include "AI.h"

#include <algorithm>

void AI::StartGame(const StartData& startData, GameRenderer::IGameRendererFactory& factory) {
    this->startData = startData;

    this->playerStartHealth = this->player.GetHealth();
    this->playerColor = RGBA8Color(0, 72, 145);
    this->playerShipSize = 0.2f;

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

    this->Division(this->GetMainRect().Right(), this->GetMainRect().Left(), this->GetMainRect().Top(), this->GetMainRect().Center().y, factory);
    this->sectorMiddle = (this->sectors.front().Left() + this->sectors.front().Right()) / 2.f;
    this->SetRespawnPointsAndCreateEnemies(factory);

    Math::Vector2 playerStart;
    playerStart.x = (this->GetMainRect().Left() + this->GetMainRect().Right()) / 2.f;
    playerStart.y = this->GetMainRect().Bottom() + (this->playerShipSize / 2.f) + 0.03f;
    this->player.SetPos(playerStart);
}

void AI::Update(float dt) {
    this->EnemyUpdate(dt);

    for (auto& g : this->gun) {
        this->Hit(g);
    }

    this->UpdateExplosions();
}

void AI::Draw(GameRenderer::IGameRenderer& renderer, float dt) {
    if (this->enemyAttackTimer > 0.f) {
        this->enemyAttackTimer -= dt;
    }
    else {
        this->enemyAttackTimer = AI::EnemyAttackInverval;
        this->EnemyAttack(renderer.GetFactory());
    }

    this->DrawPlayer(renderer);

    for (auto& enemy : this->enemyShips) {
        enemy.Draw(renderer);

        if (!enemy.GetStatus()) {
            enemy.DrawRespawnPortal(renderer);
        }
    }

    for (int i = 0; i < this->GetSectorsNum() - 1; ++i) {
        this->sectors[i].Draw(renderer);
    }

    if (!gun.empty()) {
        constexpr float SmokeLength = 0.5f;
        constexpr float BulletSpeed = 1.f;

        DrawBullets(renderer, gun[0], SmokeLength, BulletSpeed, dt, RGBA8Color(255, 255, 255));

        for (size_t i = 1; i < gun.size(); ++i) {
            DrawBullets(renderer, gun[i], SmokeLength, -BulletSpeed, dt, RGBA8Color(255, 0, 0));
        }
    }

    this->DrawExplosions(renderer, dt);
}

void AI::PlayerSetPos(float x) {
    if ((x > this->GetMainRect().Left() + this->playerShipSize / 2.f)
        && (x < this->GetMainRect().Right() - this->playerShipSize / 2.f)
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

    gun[0].emplace_back(pos, factory, this->gunSmokeTex);
}

void AI::DrawPlayer(GameRenderer::IGameRenderer& renderer) {
    auto playerBox = this->GetPlayerShipBox();

    GameRenderer::HollowRectangleGeometryParams geom;

    geom.color = this->playerColor;
    geom.thickness = 0.001f;
    geom.width = playerBox.Width();
    geom.height = playerBox.Height();

    auto transform = this->playerRectRenderer->GetRectangleTransform();

    transform.position.x = playerBox.Center().x;
    transform.position.y = playerBox.Center().y;

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

void AI::DrawExplosions(GameRenderer::IGameRenderer& renderer, float dt) {
    for (auto& i : this->explosions) {
        i.Draw(renderer, dt);
    }
}

void AI::UpdateExplosions() {
    (void)std::remove_if(std::begin(this->explosions), std::end(this->explosions), [](const Explosion& i)
        {
            return !i.IsActive();
        });
}

Explosion AI::MakeExplosion(const Math::FBox& zone, float finalRadius) const {
    Math::Vector2 randPos;

    float fltRndX = static_cast<float>(rand() % 1000) / 1000.f;
    float fltRndY = static_cast<float>(rand() % 1000) / 1000.f;

    randPos.x = zone.Left() + fltRndX * zone.Width();
    randPos.y = zone.Top() - fltRndY * zone.Height();

    return Explosion(randPos, finalRadius);
}

void AI::Hit(std::list<Bullet>& g) {
    const auto& mainRect = this->GetMainRect();

    std::erase_if(g, [&](Bullet& bullet) {
        const auto& checkPos = bullet.GetPos();

        if (!mainRect.IsInside(checkPos)) {
            return true;
        }

        auto playerShip = this->GetPlayerShipBox();
        if (playerShip.IsInside(checkPos)) {
            if (!this->player.IsImmortal()) {
                this->explosions.push_back(Explosion(checkPos, 0.05f));
                this->player.Damage(5.f);
                return true;
            }
        }
        else {
            for (auto& enShip : this->enemyShips) {
                auto enemyBox = enShip.GetEnemyRect();

                if (enemyBox.IsInside(checkPos)) {
                    this->explosions.push_back(Explosion(checkPos, 0.05f));
                    enShip.Damage(20.f);
                    return true;
                }
            }
        }

        return false;
        });
}

void AI::EnemyUpdate(float dt) {
    size_t start = 0;
    size_t end = this->GetEnemiesPerSector();
    const auto playerPos = this->player.GetPos();

    for (size_t i = 0; i < this->GetSectorsNum(); ++i) {
        const auto& sector = this->sectors[i];

        for (; start < end; ++start) {
            auto& enemy = this->enemyShips[start];

            enemy.AnimTest(playerPos.x, sector.Left(), sector.Right(), dt);
            if (!enemy.GetStatus() && enemy.GetNeedExplosion()) {
                this->kills++;
                for (int i = 0; i < 5; i++) {
                    this->explosions.push_back(this->MakeExplosion(enemy.GetEnemyRect(), 0.4f));
                }
                enemy.SetNeedExplosion(false);
            }
        }
        start = end;
        end += this->GetEnemiesPerSector();
    }
}

void AI::EnemyAttack(GameRenderer::IGameRendererFactory& factory) {
    // TODO: refactor

    const size_t enemiesPerSector = this->GetEnemiesPerSector();
    size_t enemyGunIndex = 1;

    for (size_t i = 0; i < this->enemyShips.size(); i += enemiesPerSector) {
        for (size_t k = i; k < i + enemiesPerSector; ++k) {
            auto& enemy = this->enemyShips[k];

            if (enemy.GetStatus() && !enemy.GetUnset() && enemyGunIndex < this->gun.size()) {
                this->gun[enemyGunIndex].emplace_back(enemy.GetGunPos(), factory, this->gunSmokeTex);
                ++enemyGunIndex;
                break;
            }
        }
    }
}

Math::FBox AI::GetPlayerShipBox() const {
    return Math::FBox::FromSquare(this->playerShipSize, this->player.GetPos());
}

const Math::FBox& AI::GetMainRect() const {
    return this->startData.mainRect;
}

size_t AI::GetEnemyNum() const {
    return this->startData.sectors * 3;
}

size_t AI::GetEnemiesPerSector() const {
    return this->GetEnemyNum() / this->GetSectorsNum();
}

size_t AI::GetSectorsNum() const {
    return this->startData.sectors;
}

void AI::Division(float maxCoord, float minCoord, float top, float bottom, GameRenderer::IGameRendererFactory& factory) {
    this->sectors.clear();
    this->sectors.reserve(this->GetSectorsNum());

    this->sectorLength = (maxCoord - minCoord) / static_cast<float>(this->GetSectorsNum());
    float tempLenght = this->sectorLength;
    float tempLenght2 = 0.f;
    for (size_t i = 0; i < this->GetSectorsNum(); ++i) {
        float left = maxCoord - tempLenght;
        float right = maxCoord - tempLenght2;

        this->sectors.emplace_back(Math::FBox::FromLTRB(left, top, right, bottom), factory);

        tempLenght += this->sectorLength;
        tempLenght2 += this->sectorLength;
    }
}

void AI::SetRespawnPointsAndCreateEnemies(GameRenderer::IGameRendererFactory& factory) {
    this->enemyShips.reserve(this->GetEnemyNum());
    this->respawnPoint.reserve(this->GetEnemyNum());

    const size_t enemiesPerSector = this->GetEnemiesPerSector();
    const float enemyOffsetInSector = (this->sectors.front().Top() - this->sectors.front().Bottom()) / static_cast<float>(enemiesPerSector + 1);
    size_t start = 0;
    size_t end = enemiesPerSector;
    float offsetY = enemyOffsetInSector;
    float offsetX = this->sectorMiddle;

    for (size_t i = 0; i < this->GetSectorsNum(); ++i, offsetX -= this->sectorLength) {
        for (size_t j = start; j < end; ++j, offsetY += enemyOffsetInSector) {
            this->respawnPoint.emplace_back(offsetX, sectors[i].Bottom() + offsetY);
            this->enemyShips.emplace_back(factory);

            this->enemyShips.back().SetModelPos(this->respawnPoint.back());
        }

        offsetY = enemyOffsetInSector;
        start = end;
        end += enemiesPerSector;
    }
}
