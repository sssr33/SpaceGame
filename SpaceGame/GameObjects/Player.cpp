#include "Player.h"

void Player::SetPos(const Math::Vector2& pos) {
    this->playerPos = pos;
}

void Player::SetPosX(float x) {
    this->playerPos.x = x;
}

const Math::Vector2& Player::GetPos() const {
    return this->playerPos;
}

void Player::Damage(float damage) {
    if (this->health > 0.f && !this->immortal) {
        this->health -= damage;
    }
}

float Player::GetHealth() const {
    return this->health;
}
