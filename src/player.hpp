#pragma once
#include "render.hpp"
#include "raylib.h"
#include <functional>
namespace rl = raylib;

class Player {
    private:
    std::shared_ptr<Light> light;
    public:
    rl::Vector2 pos;

    Player(std::shared_ptr<Light> light):
        pos(rl::Vector2{0, 0}),
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

    std::function<float()> getX;
    std::function<float()> getY;
    std::function<rl::Vector2()> getPos;
};
