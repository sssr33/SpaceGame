#pragma once
#include "Renderer/GameRenderer/IGameRenderer.h"
#include "Renderer/GameRenderer/IRectangleRenderer.h"

#include <vector>
#include <libhelpers/Math/Box.h>
#include <libhelpers/Math/Vector.h>

class Enemy {
public:
    explicit Enemy(GameRenderer::IGameRendererFactory& factory);

    // draw_enemy
    void Draw(GameRenderer::IGameRenderer& renderer);
    // Draw_respawn_portal
    void DrawRespawnPortal(GameRenderer::IGameRenderer& renderer, float dt);

    // set_model_pos
    void SetModelPos(const Math::Vector2& pos);
    // anim_test
    void AnimTest(float x, float lim1, float lim2, float dt);
    // get_gun_pos
    Math::Vector2 GetGunPos();
    // get_status
    bool GetStatus() const;
    // unset
    bool GetUnset() const;

    // need_explosion
    bool GetNeedExplosion() const;
    // need_explosion
    void SetNeedExplosion(bool need);

    // get_enemy_rect
    Math::FBox GetEnemyRect() const;
    // damage
    void Damage(float damage);

    // after_respawn_sync
    //void after_respawn_sync(RECT, bool);

    // get_direction
    bool GetDirection();
    // set_direction
    void SetDirection(size_t direct);
    // get_xy
    Math::Vector2 GetCenter();
    // respawn
    void Respawn(const Math::Vector2& pos, size_t direct);
    // respawned
    bool IsRespawned() const;

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
    int portalPulseCount = 0;
    float respawnTime = 0.f; // respawn_time
    float respawnTimer = 0.f;
    bool portalStarted = false;

    bool respawned = false;
    bool needExplosion = true; // need_explosion
    bool unset = false;

    bool status = true;
    bool targetDetect = true; // target_detect

    std::shared_ptr<GameRenderer::IRectangleRenderer> modelRenderer;
    std::shared_ptr<GameRenderer::IRectangleRenderer> modelFillRenderer;
    std::shared_ptr<GameRenderer::IRectangleRenderer> modelCenterDotRenderer;

    std::shared_ptr<GameRenderer::IRectangleRenderer> portalInnerFillRenderer;
    std::shared_ptr<GameRenderer::IRectangleRenderer> portalOuterFillRenderer;
};
