#pragma once

#include <string>
#include <math.h>

using std::string;

// the same as @as in zig
// it's personally more readable
#define as(ty, expr) ((ty)expr)
// clearer syntax for function ptr types
#define fn(ret, args...) ret(*)(args)
// makes it easier to pass a pair to a function
#define pairToParts(pair) pair.first, pair.second
// clearer purpose
#define ABSTRACT 0

#if defined(DEBUG)
#include <iostream>
#include <source_location>
#endif
namespace debug {
    #if !defined(DEBUG)
    // acts as a /dev/null for logging in release builds
    // doesn't consume std::endl by design
    struct NullStream {
        template <typename T>
        NullStream& operator<<(const T&) { return *this; }
    };
    #endif
    // prefer to use this over std::endl
    const char endl = '\n';

    inline
    #if defined(DEBUG)
    std::ostream&
    #else
    NullStream&
    #endif
    print_stream(
        #if defined(DEBUG)
        const std::source_location callsite = std::source_location::current()
        #endif
    ) {
        #if defined(DEBUG)
        std::clog
            <<
            #if defined(PRETTY_LOGS)
            "\e[0;36m"
            #endif
            "debug::print_stream from "
            << callsite.file_name()
            << '('
            << callsite.line()
            << ':'
            << callsite.column()
            << "): "
            #if defined(PRETTY_LOGS)
            "\e[0m"
            #endif
            ;
        return std::clog;
        #else
        static NullStream null_stream;
        return null_stream;
        #endif
    }

    template<typename T>
    inline void println(
        const T& val
        #if defined(DEBUG)
        , const std::source_location callsite = std::source_location::current()
        #endif
    ) {
        #if defined(DEBUG)
        std::clog
            <<
            #if defined(PRETTY_LOGS)
            "\e[0;36m"
            #endif
            "debug::println from "
            << callsite.file_name()
            << '('
            << callsite.line()
            << ':'
            << callsite.column()
            << ")"
            << "\n"
            #if defined(PRETTY_LOGS)
            "\e[0;33m"
            #endif
            << val
            <<
            #if defined(PRETTY_LOGS)
            "\e[0m"
            #endif
            "\n";
        #endif
    }

}

#define PLACEHOLDER_TYPE void()
