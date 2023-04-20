#pragma once
#include "Player.h"
#include "Bullet.h"
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

    Math::FBox GetPlayerShipBox() const; // player_ship

    const Math::FBox& GetMainRect() const; // mainRect

    StartData startData;

    Player player;
    RGBA8Color playerColor; // player_color
    float playerShipSize; // player_ship_size
    float playerStartHealth; // pl_start_health
    int kills;
    int maxKills; // max_kills
    std::shared_ptr<GameRenderer::IRectangleRenderer> playerRectRenderer;
    std::shared_ptr<GameRenderer::IRectangleRenderer> playerRectFillRenderer;

    std::vector<std::list<Bullet>> gun;
    std::shared_ptr<GameRenderer::ITexture2D> gunSmokeTex;
};
