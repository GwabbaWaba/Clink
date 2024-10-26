#pragma once

#if defined(DEBUG)
#include <iostream>
#endif

#include <map>
#include <vector>
#include <math.h>
#include "bitsizeints.h"

#define as(ty, expr) ((ty)expr)
#define fn(ret, args...) ret(*)(args)

namespace debug {
    template<typename T>
    inline void print(const T& val) {
        #if defined(DEBUG) 
        std::clog << val;
        #endif
    }
    template<typename T>
    inline void println(const T& val) {
        #if defined(DEBUG) 
        std::clog << val << std::endl;
        #endif
    }
}

#define PLACEHOLDER_TYPE void()

struct World {
    std::map<std::pair<i32, i32>, std::vector<PLACEHOLDER_TYPE>> machine_grid;
    std::map<std::pair<i32, i32>, PLACEHOLDER_TYPE> terrain_grid;
};
