#pragma once

#include <libhelpers/Math/Vector.h>

class Player {
public:
    void SetPos(const Math::Vector2& pos);
    void SetPosX(float x);
    const Math::Vector2& GetPos() const;
    void Damage(float damage);
    float GetHealth() const;

private:
    Math::Vector2 playerPos = { 0.f, 0.f };
    float health = 100.f;
    bool immortal = false;
};
