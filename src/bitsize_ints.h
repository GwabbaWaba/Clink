#pragma once

#include <stddef.h>
#include <stdint.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define usize size_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define f32 float
#define f64 double


#ifdef __SIZEOF_INT128__
    #define u128 __uint128_t
    #define i128 __int128_t
#else
    typedef unsigned long long u128;
    typedef signed long long u128;
#endif

#if defined(__cplusplus)
namespace bitsize_literals {
    #define INT_LITERAL_OPERATOR(type) \
    constexpr type operator"" _##type (unsigned long long literal) { return (type)literal; }

    INT_LITERAL_OPERATOR(u8)
    INT_LITERAL_OPERATOR(u16)
    INT_LITERAL_OPERATOR(u32)
    INT_LITERAL_OPERATOR(u64)
    INT_LITERAL_OPERATOR(u128)
    INT_LITERAL_OPERATOR(usize)

    INT_LITERAL_OPERATOR(i8)
    INT_LITERAL_OPERATOR(i16)
    INT_LITERAL_OPERATOR(i32)
    INT_LITERAL_OPERATOR(i64)
    INT_LITERAL_OPERATOR(i128)

    #undef INT_LITERAL_OPERATOR
}
#endif
