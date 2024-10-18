#pragma once

#include <iostream>
#include "global.hpp"

#define as(ty, expr) ((ty)expr)
#define fn(ret, args...) ret(*)(args)

#define print(expr) std::clog << expr
#define println(expr) std::clog << expr << std::endl

struct World {
    f32 width;
    f32 height;
    f32 depthLimit;
    f32 heightLimit;
    
};
