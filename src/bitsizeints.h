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
