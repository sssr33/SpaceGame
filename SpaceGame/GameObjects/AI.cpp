#include "AI.h"

#include <algorithm>

void AI::StartGame(const StartData& startData, GameRenderer::IGameRendererFactory& factory, Stats* gameStat) {
    this->startData = startData;
    this->gameStat = gameStat;

    this->maxKills = 20 + rand() % 30;

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

    {
        const uint32_t width = 128;
        const uint32_t height = width;

        const float maxDist = 0.5f;
        const float minDist = maxDist * 0.5f;
        const float distRange = maxDist - minDist;
        Math::Vector2 center = { 0.5f, 0.5f };

        std::vector<RGBA8Color> texData(width * height, RGBA8Color(255, 255, 255, 0));

        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                uint32_t idx = y * width + x;

                Math::Vector2 point = { static_cast<float>(x) / static_cast<float>(width - 1), static_cast<float>(y) / static_cast<float>(height - 1) };
                const float dist = (point - center).Length();
                const float rangedDist = dist - minDist;
                const float alphaF = std::clamp(rangedDist / distRange, 0.f, 1.f);
                uint8_t alpha = static_cast<uint8_t>(alphaF * 255.f);

                texData[idx] = RGBA8Color(255, 255, 255, alpha);
            }
        }

        this->playerImmortalFieldTex = factory.MakeTexture2DFromMemory(width, height, GameRenderer::TexturePixelFormat::BGRA8, texData.data(), texData.size() * sizeof(RGBA8Color));
    }

    this->playerImmortalFieldRenderer = factory.MakeRectangleRenderer();
    {
        GameRenderer::FilledRectangleGeometryParams fillGeom;

        fillGeom.color = RGBA8Color(0, 0, 255, 100);
        fillGeom.width = 1.f;
        fillGeom.height = 1.f;
        fillGeom.roundness = 1.f;

        this->playerImmortalFieldRenderer->SetGeometryParams(fillGeom);
    }
}

void AI::Update(float dt) {
    this->StatUpdate();
    this->EnemyUpdate(dt);

    for (auto& g : this->gun) {
        this->Hit(g);
    }

    this->UpdateExplosions();
    this->EnemyRespawnInSector();
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
            enemy.DrawRespawnPortal(renderer, dt);
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

void AI::TogglePlayerImmortal() {
    this->player.SetImmortal(!this->player.IsImmortal());
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

    if (this->player.IsImmortal()) {
        auto fieldTransform = transform;
        const float scale = 0.7f;

        fieldTransform.scale = { scale, scale };

        this->playerImmortalFieldRenderer->SetRectangleTransform(fieldTransform);
        renderer.RenderRectangle(this->playerImmortalFieldRenderer, this->playerImmortalFieldTex);
    }
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

void AI::EnemyRespawnInSector() {
    if (this->kills < this->maxKills) {
        for (size_t i = 0; i < this->sectors.size(); ++i) {
            size_t sectorShipStartIdx = i * this->GetEnemiesPerSector();
            size_t sectorShipEndIdx = (i + 1) * this->GetEnemiesPerSector();

            bool isAllEnemiesInSectorReadyForRespawn = std::all_of(
                std::begin(this->enemyShips) + sectorShipStartIdx,
                std::begin(this->enemyShips) + sectorShipEndIdx,
                [](const Enemy& enemy)
                {
                    return enemy.IsRespawned();
                });

            if (isAllEnemiesInSectorReadyForRespawn) {
                size_t direction = static_cast<size_t>(rand()) % 1;
                float posX = (this->sectors[i].Left() + this->sectors[i].Right()) / 2.f;

                for (size_t j = sectorShipStartIdx; j < sectorShipEndIdx; ++j) {
                    auto pos = this->enemyShips[j].GetCenter();
                    pos.x = posX;
                    this->enemyShips[j].Respawn(pos, direction);
                }
            }
        }
    }
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

void AI::StatUpdate() {
    if (this->gameStat) {
        constexpr auto ColorYellow = RGBA8Color(255, 194, 63);
        constexpr auto ShiledColorRed = RGBA8Color(255, 40, 2);
        constexpr auto CaseColorLightBlue = RGBA8Color(181, 203, 255);
        constexpr auto ShieldColorBlue = RGBA8Color(53, 90, 255);
        constexpr auto CaseColorRed = RGBA8Color(198, 31, 1);
        constexpr auto CaseColorBlack = RGBA8Color(0, 0, 0);
        constexpr auto ShieldColorGray = RGBA8Color(128, 128, 128);
        const float playerHealth = this->player.GetHealth();

        if (this->player.IsImmortal()) {
            this->gameStat->SetCaseColor(CaseColorLightBlue);
            this->gameStat->SetShieldColor(ShieldColorBlue);
        }
        else if (playerHealth > 85.f) {
            this->gameStat->SetCaseColor(Stats::CaseColorGreen);
            this->gameStat->SetShieldColor(Stats::ShieldColorGreen);
        }
        else if (playerHealth > 70.f) {
            this->gameStat->SetCaseColor(Stats::CaseColorGreen);
            this->gameStat->SetShieldColor(ColorYellow);
        }
        else if (playerHealth > 40.f) {
            this->gameStat->SetCaseColor(Stats::CaseColorGreen);
            this->gameStat->SetShieldColor(ShiledColorRed);
        }
        else if (playerHealth > 25.f) {
            this->gameStat->SetCaseColor(ColorYellow);
            this->gameStat->SetShieldColor(ShiledColorRed);
        }
        else if (playerHealth > 0.f) {
            this->gameStat->SetCaseColor(CaseColorRed);
            this->gameStat->SetShieldColor(ShiledColorRed);
        }
        else {
            this->gameStat->SetCaseColor(CaseColorBlack);
            this->gameStat->SetShieldColor(ShieldColorGray);
        }
    }
}

Math::FBox AI::GetPlayerShipBox() const {
    return Math::FBox::FromSquare(this->playerShipSize, this->player.GetPos());
}

const Math::FBox& AI::GetMainRect() const {
    return this->startData.mainRect;
}

size_t AI::GetEnemiesPerSector() const {
    return 3;
}

size_t AI::GetEnemyNum() const {
    return this->startData.sectors * this->GetEnemiesPerSector();
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
