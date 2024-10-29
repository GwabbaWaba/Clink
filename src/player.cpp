#include "player.hpp"
#include "raylib.h"

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
}

void Player::updateLight() {
    this->light->move(this->pos.x, this->pos.y);
}
