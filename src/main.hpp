#pragma once
#include <map>
#include <vector>
#include "bitsizeints.h"

#define as(ty, expr) ((ty)expr)
#define fn(ret, args...) ret(*)(args)

#define print(expr) std::clog << expr
#define println(expr) std::clog << expr << std::endl

#define PLACEHOLDER_TYPE int

struct World {
    std::map<std::pair<i32, i32>, std::vector<PLACEHOLDER_TYPE>> machine_grid;
    std::map<std::pair<i32, i32>, PLACEHOLDER_TYPE> terrain_grid;
};
