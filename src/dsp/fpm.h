#pragma once

#include <cstdint>
#include <cmath>
#include "findtype.h"

namespace fpm
{
    inline constexpr int8_t calc_frac_bits(int8_t width, int8_t start_bit, int8_t sign_bits)
    {
        if (start_bit >= 0)
            return 0;
        if (start_bit <= -width)
            if (sign_bits)
                return width - 1;
            else
                return width;
        return -start_bit;
    }

    // fields explanation
    // width       -- storage width in bits
    // sign_bits   -- 1 for signed, 0 for unsigned
    // start_bit   -- the first bit has a value of pow(2, start_bit)
    //                negative means that there are fractional bits to the left of the integer part
    //                zero means it's a normal integer
    template <int8_t _width, int8_t _start_bit, bool _signed>
    struct descr
    {
    public:
        //user defined fields
        static constexpr int8_t width = _width;
        static constexpr int8_t sign_bits = (_signed ? 1 : 0);
        static constexpr int8_t start_bit = _start_bit;

        //calculated fields
        static constexpr int8_t frac_bits = calc_frac_bits(width, sign_bits, start_bit);
        static constexpr int8_t int_bits = (_width - sign_bits - frac_bits);
        typedef typename findType<width, _signed>::type t;
        typedef typename findType<width, false>::type u;
        typedef typename findType<width, true>::type s;
        
        static constexpr bool is_signed = _signed;
        static constexpr t max = std::numeric_limits<t>::max();
        static constexpr t min = std::numeric_limits<t>::min();
        static constexpr double epsilon_d = pow(2, start_bit);
        static constexpr float epsilon = (float)epsilon_d;
    };

    using int8_descr = descr<8, 0, true>;
    using uint8_descr = descr<8, 0, false>;
    using int16_descr = descr<16, 0, true>;
    using uint16_descr = descr<16, 0, false>;
    using int32_descr = descr<32, 0, true>;
    using uint32_descr = descr<32, 0, false>;
    using int64_descr = descr<64, 0, true>;
    using uint64_descr = descr<64, 0, false>;

    using q7_descr = descr<8, -7, true>;
    using q15_descr = descr<16, -15, true>;
    using q31_descr = descr<32, -31, true>;
    using q63_descr = descr<64, -63, true>;

// disable shift overflow warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-count-overflow"
    template<typename T, int bits>
    inline constexpr T shift(T n)
    {
        if (bits == 0)
            return n;
        if (abs(bits) >= sizeof(T) * 8)
            return 0;
        if (bits > 0)
            return (n << bits);
        else
            return (n >> (-bits));
    }
#pragma GCC diagnostic pop

    template<typename R, typename S>
    inline constexpr auto conv(typename S::t s) -> typename R::t
    {
        static_assert(std::is_base_of<descr<R::width, R::start_bit, R::is_signed>, R>::value, "R must be an instantiation of descr");
        static_assert(std::is_base_of<descr<S::width, S::start_bit, S::is_signed>, S>::value, "S must be an instantiation of descr");

        if (R::width == S::width && R::start_bit == S::start_bit && R::is_signed == S::is_signed)
            return s;
        else if (R::width >= S::width)
            return shift<typename R::t, S::start_bit - R::start_bit>(s);
        else
            return shift<typename S::t, S::start_bit - R::start_bit>(s);
    }

    template<typename S>
    inline constexpr float to_float(typename S::t s)
    {
        static_assert(std::is_base_of<descr<S::width, S::start_bit, S::is_signed>, S>::value, "S must be an instantiation of descr");

        return (float)s * S::epsilon;
    }

    template<typename R>
    inline constexpr auto from_float(float s) -> typename R::t
    {
        static_assert(std::is_base_of<descr<R::width, R::start_bit, R::is_signed>, R>::value, "R must be an instantiation of descr");
        const float ret = (s / R::epsilon);
        return (typename R::t) ret;
    }

    template<typename S>
    inline constexpr double to_double(typename S::t s)
    {
        static_assert(std::is_base_of<descr<S::width, S::start_bit, S::is_signed>, S>::value, "S must be an instantiation of descr");

        return (double)s * S::epsilon_d;
    }

    template<typename R>
    inline constexpr auto from_double(double s) -> typename R::t
    {
        return (typename R::t) (s / R::epsilon_d);
    }

    template<typename A, typename B>
    struct mul_result_struct : public descr<validWidth(A::width + B::width), A::start_bit + B::start_bit, (A::is_signed || B::is_signed)> {};

    template<typename R, typename A, typename B>
    inline constexpr auto mul(typename A::t a, typename B::t b) -> typename R::t
    {
        using tmpType = mul_result_struct<A, B>;
        return conv<R, tmpType>((typename tmpType::t)a * b);
    }

    //TODO division
}