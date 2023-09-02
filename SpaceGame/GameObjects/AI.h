#pragma once
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Sector.h"
#include "Explosion.h"
#include "Stats.h"
#include "Renderer/GameRenderer/IGameRenderer.h"
#include "Renderer/GameRenderer/IRectangleRenderer.h"

#include <vector>
#include <list>
#include <libhelpers/Math/Box.h>

/*
* ::TODO::
* stat_control
* player_immortal
* set_player_color
* unfinished boos logic: start_boss_demo, boss_setup, boss_portal_draw
* player destroy logic
*/
class AI {
public:
    struct StartData {
        size_t sectors = 0;
        Math::FBox mainRect;
    };

    // start_game
    void StartGame(const StartData& startData, GameRenderer::IGameRendererFactory& factory, Stats* gameStat);

    // main_calc
    void Update(float dt);

    // main_draw
    void Draw(GameRenderer::IGameRenderer& renderer, float dt);

    // player_pos_set
    void PlayerSetPos(float x);
    // player_gun_shoot
    void PlayerGunShot(GameRenderer::IGameRendererFactory& factory);

private:
    // draw_player
    void DrawPlayer(GameRenderer::IGameRenderer& renderer);
    // draw_player_fire
    void DrawPlayerFire(GameRenderer::IGameRenderer& renderer);

    // draw_bullets
    void DrawBullets(GameRenderer::IGameRenderer& renderer, std::list<Bullet>& g, float smokeLenght, float speed, float dt, RGBA8Color smokeColor);

    // draw_explosions
    void DrawExplosions(GameRenderer::IGameRenderer& renderer, float dt);
    // calc_explosions
    void UpdateExplosions();
    // enemy_respawn_in_sector
    void EnemyRespawnInSector();
    // get_expl
    Explosion MakeExplosion(const Math::FBox& zone, float finalRadius) const;

    // hit
    void Hit(std::list<Bullet>& g);

    // enemy_control
    void EnemyUpdate(float dt);
    // enemy_attack
    void EnemyAttack(GameRenderer::IGameRendererFactory& factory);

    // stat_control
    void StatUpdate();

    // player_ship
    Math::FBox GetPlayerShipBox() const;

    // mainRect
    const Math::FBox& GetMainRect() const;

    size_t GetEnemiesPerSector() const;
    size_t GetEnemyNum() const;
    // sectors_num
    size_t GetSectorsNum() const;

    // division
    void Division(float maxCoord, float minCoord, float top, float bottom, GameRenderer::IGameRendererFactory& factory);
    // set_respawn_points
    void SetRespawnPointsAndCreateEnemies(GameRenderer::IGameRendererFactory& factory);

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

    std::list<Explosion> explosions;

    // non owning ptr
    Stats* gameStat = nullptr;
};
