#include "player.hpp"
#include "raylib.h"

namespace rl = raylib;

PlayerAPI::PlayerAPI(Player& player) {
    this->moveX = [&](float x) {
        player.pos.x += x;
        player.updateLight();
    };
    this->moveY = [&](float y) {
        player.pos.y += y;
        player.updateLight();
    };
    this->moveTo = [&](float x, float y) {
        player.pos.x = x;
        player.pos.y = y;
        player.updateLight();
    };

    this->getX = [&]() -> float {
        return player.pos.x;
    };
    this->getY = [&]() -> float {
        return player.pos.y;
    };
    this->getPos = [&]() -> rl::Vector2 {
        return player.pos;
    };
}

void Player::updateLight() {
    this->light->move(this->pos.x, this->pos.y);
}
