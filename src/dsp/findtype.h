#pragma once

#include <cstdint>
#include <cmath>

#define MAX_WIDTH 64
#define MIN_WIDTH 8

namespace fpm
{
    constexpr int8_t validWidth(int8_t bits)
    {
        if (bits <= 8)
            return 8;
        if (bits <= 16)
            return 16;
        if (bits <= 32)
            return 32;
        return 64;
    }

    template <int8_t width, bool _signed>
    struct findType
    { };

    template <>
    struct findType<8, true>
    {
    public:
        typedef int8_t type;
    };

    template <>
    struct findType<8, false>
    {
    public:
        typedef uint8_t type;
    };

    template <>
    struct findType<16, true>
    {
    public:
        typedef int16_t type;
    };

    template <>
    struct findType<16, false>
    {
    public:
        typedef uint16_t type;
    };

    template <>
    struct findType<32, true>
    {
    public:
        typedef int32_t type;
    };

    template <>
    struct findType<32, false>
    {
    public:
        typedef uint32_t type;
    };

    template <>
    struct findType<64, true>
    {
    public:
        typedef int64_t type;
    };

    template <>
    struct findType<64, false>
    {
    public:
        typedef uint64_t type;
    };
  
}