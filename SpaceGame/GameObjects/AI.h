#pragma once
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Sector.h"
#include "Renderer/GameRenderer/IGameRenderer.h"
#include "Renderer/GameRenderer/IRectangleRenderer.h"

#include <vector>
#include <list>
#include <libhelpers/Math/Box.h>

class AI {
public:
    struct StartData {
        size_t sectors = 0;
        Math::FBox mainRect;
    };

    void StartGame(const StartData& startData, GameRenderer::IGameRendererFactory& factory); // start_game

    void Update(float dt); // main_calc

    void Draw(GameRenderer::IGameRenderer& renderer, float dt); // main_draw

    void PlayerSetPos(float x); // player_pos_set
    void PlayerGunShot(GameRenderer::IGameRendererFactory& factory); // player_gun_shoot

private:
    void DrawPlayer(GameRenderer::IGameRenderer& renderer); // draw_player
    void DrawPlayerFire(GameRenderer::IGameRenderer& renderer); // draw_player_fire

    void DrawBullets(GameRenderer::IGameRenderer& renderer, std::list<Bullet>& g, float smokeLenght, float speed, float dt, RGBA8Color smokeColor); // draw_bullets

    void Hit(std::list<Bullet>& g); // hit

    void EnemyUpdate(float dt); // enemy_control

    void EnemyAttack(GameRenderer::IGameRendererFactory& factory);

    Math::FBox GetPlayerShipBox() const; // player_ship

    const Math::FBox& GetMainRect() const; // mainRect

    size_t GetEnemyNum() const;
    size_t GetEnemiesPerSector() const;
    size_t GetSectorsNum() const;// sectors_num

    void Division(float maxCoord, float minCoord, float top, float bottom, GameRenderer::IGameRendererFactory& factory); // division
    void SetRespawnPointsAndCreateEnemies(GameRenderer::IGameRendererFactory& factory); // set_respawn_points

    static constexpr float EnemyAttackInverval = 0.8f; // 800ms

    StartData startData;

    Player player;
    RGBA8Color playerColor; // player_color
    float playerShipSize = 0.f; // player_ship_size
    float playerStartHealth = 0.f; // pl_start_health
    int kills = 0;
    int maxKills = 0; // max_kills
    std::shared_ptr<GameRenderer::IRectangleRenderer> playerRectRenderer;
    std::shared_ptr<GameRenderer::IRectangleRenderer> playerRectFillRenderer;

    std::vector<std::list<Bullet>> gun;
    std::shared_ptr<GameRenderer::ITexture2D> gunSmokeTex;

    std::vector<Sector> sectors;
    float sectorMiddle = 0.f; // sector_middle
    float sectorLength = 0.f; // sector_lenght

    std::vector<Math::Vector2> respawnPoint; // respawn_point
    std::vector<Enemy> enemyShips; // en_ship
    float enemyAttackTimer = AI::EnemyAttackInverval;
};
