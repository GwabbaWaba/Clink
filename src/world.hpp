#pragma once
#include <bitset>
#include <map>
#include <memory>
#include <vector>
#include "main.hpp"
#include "bitsize_ints.h"

using namespace bitsize_literals;

typedef std::pair<i32, i32> Position;

class Machine {
    private:
    u32 occupancy = ~0_u32;
    public:
    Position pos; // top left
    i32 width;
    i32 height;

    void update();
    bool occupiesSpace(i32 x, i32 y);
    Position worldPosToMachineRelative(i32 world_x, i32 world_y);
};

class Terrain {
    
};

class World {
    public:
    std::map<Position, std::vector<std::shared_ptr<Machine>>> machine_grid;
    std::map<Position, std::vector<std::shared_ptr<Terrain>>> terrain_grid;
};
