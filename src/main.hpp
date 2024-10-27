#pragma once

#if defined(DEBUG)
#include <iostream>
#endif

#include <string>
#include <math.h>

using std::string;

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
#define DECL_ENUM_ELEMENT( element ) #element
#define BEGIN_ENUM( ENUM_NAME ) char* gs_##ENUM_NAME [] =
#define END_ENUM( ENUM_NAME ) ; char* getString##ENUM_NAME(enum \
        tag##ENUM_NAME index){ return gs_##ENUM_NAME [index]; }

#define PLACEHOLDER_TYPE void()
