#pragma once

#if defined(DEBUG)
#include <iostream>
#endif

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <math.h>
#include "bitsizeints.h"

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

namespace clink_exceptions {
    #define EXCEPTION_TYPE_BRANCHES \
        X(invalid_access)
    
    #define X(name) name,
    enum EventExceptionType {
        EXCEPTION_TYPE_BRANCHES
    };
    #undef X

    class EventException : public std::exception {
        private:
        #define X(name) #name,
        static constexpr const char* strExceptionType[] = {
            EXCEPTION_TYPE_BRANCHES
        };
        #undef X

        const string message;

        public:
        EventException(EventExceptionType type, string message):
            message(string(strExceptionType[type]) + message)
        {}
        const char* what() {
            return this->message.c_str();
        }

    };

    #undef EXCEPTION_TYPE_BRANCHES
}

struct World {
    std::map<std::pair<i32, i32>, std::vector<PLACEHOLDER_TYPE>> machine_grid;
    std::map<std::pair<i32, i32>, PLACEHOLDER_TYPE> terrain_grid;
};

