#pragma once

#if defined(DEBUG)
#include <iostream>
#endif

#include <string>
#include <math.h>

using std::string;

#define as(ty, expr) ((ty)expr)
#define fn(ret, args...) ret(*)(args)
#define ABSTRACT 0

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
