#pragma once
#include "render.hpp"
#include "raylib.h"
#include <functional>
namespace rl = raylib;

class Player {
    private:
    Light* light;
    public:
    rl::Vector2 pos;

    Player(Light* light):
        light(light)
    {}

    void updateLight();
};

struct PlayerAPI {
    public:
    PlayerAPI(Player& player);

    std::function<void(float x)> moveX;
    std::function<void(float y)> moveY;
    std::function<void(float x, float y)> moveTo;
};
