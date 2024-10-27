#pragma once
#include <map>
#include <vector>
#include "main.hpp"
#include "bitsize_ints.h"

class World {
    public:
    std::map<std::pair<i32, i32>, std::vector<PLACEHOLDER_TYPE>> machine_grid;
    std::map<std::pair<i32, i32>, PLACEHOLDER_TYPE> terrain_grid;
};
