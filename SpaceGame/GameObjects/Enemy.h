#pragma once
#include "Renderer/GameRenderer/IGameRenderer.h"
#include "Renderer/GameRenderer/IRectangleRenderer.h"

#include <vector>
#include <libhelpers/Math/Box.h>
#include <libhelpers/Math/Vector.h>

class Enemy {
public:
    explicit Enemy(GameRenderer::IGameRendererFactory& factory);

    void Draw(GameRenderer::IGameRenderer& renderer); // draw_enemy
    void DrawRespawnPortal(GameRenderer::IGameRenderer& renderer); // Draw_respawn_portal

    void SetModelPos(const Math::Vector2& pos); // set_model_pos
    void AnimTest(float x, float lim1, float lim2, float dt); // anim_test
    Math::Vector2 GetGunPos(); // get_gun_pos
    bool GetStatus() const; // get_status

    bool GetNeedExplosion() const; // need_explosion
    void SetNeedExplosion(bool need); // need_explosion

    Math::FBox GetEnemyRect() const; // get_enemy_rect
    void Damage(float damage); // damage
    //void after_respawn_sync(RECT, bool); // after_respawn_sync
    bool GetDirection(); // get_direction
    void SetDirection(size_t direct); // set_direction
    Math::Vector2 GetCenter(); // get_xy
    void Respawn(const Math::Vector2& pos, size_t direct); // respawn

private:
    class Switch {
    public:
        explicit Switch(size_t size);

        void SetActive(size_t index);
        bool IsActive(size_t index) const;

    private:
        std::vector<bool> values;
    };

    float health = 100.f;
    Switch direction;
    Math::FBox model;

    float portalPulse = 0.f; // portal_pulse
    float respawnTime = 0.f; // respawn_time

    bool respawned = false;
    bool needExplosion = true; // need_explosion
    bool unset = false;

    bool status = true;
    bool targetDetect = true; // target_detect

    std::shared_ptr<GameRenderer::IRectangleRenderer> modelRenderer;
    std::shared_ptr<GameRenderer::IRectangleRenderer> modelFillRenderer;
    std::shared_ptr<GameRenderer::IRectangleRenderer> modelCenterDotRenderer;
};
