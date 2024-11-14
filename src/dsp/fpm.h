#pragma once

#include <cstdint>
#include <cmath>

#ifndef FPM_WIDTH
#define FPM_WIDTH 16
#endif

#define FPM_NATIVE_MULT

namespace dsp
{
    namespace fpm
    {

#if FPM_WIDTH == 16
        using half = int8_t;
        using base = int16_t;
        using extended = int32_t;
        using uhalf = uint8_t;
        using ubase = uint16_t;
        using uextended = uint32_t;
        static constexpr int8_t width = 16;
        static constexpr float f_shift = 65536.0f;
        static constexpr float f_shift_half = 256.0f;
        static constexpr base shift_half = 256;
        static constexpr extended shift = 65536;
#elif FPM_WIDTH == 32
        using half = int16_t;
        using base = int32_t;
        using extended = int64_t;
        using uhalf = uint16_t;
        using ubase = uint32_t;
        using uextended = uint64_t;
        static constexpr int8_t width = 32;
        static constexpr float f_shift = 4294967296.0f;
        static constexpr float f_shift_half = 65536.0f;
        static constexpr base shift_half = 65536;
        static constexpr extended shift = 4294967296;
#else
#error "Unsupported width"
#endif

        // think of a fixed point integer of type "extended", with width bits on each side of the decimal point (1 of them being the sign bit)
        // base variables with prefix e represent the extended variable
        // base variables with prefix u represent the upper half of the extended variable --> the integer part
        // base variables with prefix l represent the lower half of the extended variable --> the fractional part
        // base variables with prefix m represent the middle part of the extended variable --> another fixed point number with
        //       width/2 bits on each side of the decimal point
        //  extended has the bits like this:
        //  <sign : 1><upper : width -1><lower : width -1><extra : 1>
        //  <sign : 1><pad : width/2><middle : width><pad : width/2-1>

        // math functions are defined for mixed arguments and have the following naming convention:
        //       <output type>_<operator>_<input types>

        // conversion

        inline constexpr extended e_from_e(extended e0)
        {
            return e0;
        }

        inline constexpr base u_from_u(base u0)
        {
            return u0;
        }

        inline constexpr base m_from_m(base m0)
        {
            return m0;
        }

        inline constexpr base l_from_l(base l0)
        {
            return l0;
        }

        inline constexpr base al_from_al(base al0)
        {
            return al0;
        }

        inline constexpr extended e_from_au(ubase au)
        {
            return (extended)au << (width);
        }

        inline constexpr extended e_from_am(ubase am)
        {
            return (extended)am << (width / 2);
        }

        inline constexpr extended e_from_al(ubase al)
        {
            return (extended)al;
        }

        inline constexpr ubase al_from_ae(uextended ae)
        {
            return (ubase)ae;
        }

        inline constexpr ubase al_from_e(extended e)
        {
            return (ubase)e;
        }

        inline constexpr extended e_from_u(base u)
        {
            return (extended)u << (width);
        }

        inline constexpr extended e_from_m(base m)
        {
            return (extended)m << (width / 2);
        }

        inline constexpr base m_from_e(extended m)
        {
            return (base)(m >> (width / 2));
        }

        inline constexpr extended e_from_l(base l)
        { // CAREFUL FOR THE SHIFT
            return (extended)l * 2;
        }

        inline constexpr base l_from_e(extended e)
        { // CAREFUL FOR THE SHIFT
            return (base)e / 2;
        }

        inline constexpr base m_from_l(base l)
        { // CAREFUL FOR THE SHIFT
            return l >> (width / 2 - 1);
        }

        inline constexpr base l_from_al(ubase al)
        { // CAREFUL FOR THE SHIFT
            return al >> 1;
        }

        inline constexpr ubase al_from_l(base l)
        { // CAREFUL FOR THE SHIFT
            return (ubase)l << 1;
        }

        inline constexpr base l_from_m(base m)
        { // CAREFUL FOR THE SHIFT
            return m << (width / 2 - 1);
        }

        inline constexpr float f_from_al(ubase al)
        { // CAREFUL FOR THE SHIFT
            return (float)al / f_shift;
        }

        inline constexpr float f_from_l(base l)
        { // CAREFUL FOR THE SHIFT
            return (float)l / f_shift * 2;
        }

        inline constexpr base l_from_f(float f)
        { // CAREFUL FOR THE SHIFT
            return (base)(f * f_shift / 2);
        }

        inline constexpr ubase al_from_f(float f)
        { // CAREFUL FOR THE SHIFT
            return (ubase)(f * f_shift);
        }

        inline constexpr double d_from_l(base l)
        { // CAREFUL FOR THE SHIFT
            return (double)l / f_shift * 2;
        }

        inline constexpr base l_from_d(double d)
        { // CAREFUL FOR THE SHIFT
            return (base)(d * f_shift / 2);
        }

        inline constexpr double d_from_al(ubase al)
        { // CAREFUL FOR THE SHIFT
            return (double)al / f_shift;
        }

        inline constexpr ubase al_from_d(double d)
        { // CAREFUL FOR THE SHIFT
            return (ubase)(d * f_shift);
        }

        inline constexpr base u_from_l(base l)
        {
            return 0;
        }

        inline constexpr base l_from_u(base u)
        {
            return 0;
        }

        inline constexpr base u_from_e(extended e)
        {
            return e >> width;
        }

        inline constexpr base u_from_m(base m)
        {
            return m >> (width / 2);
        }

        inline constexpr base m_from_u(base u)
        {
            return u << (width / 2);
        }

        inline constexpr float f_from_e(extended e)
        {
            return (float)e / f_shift;
        }

        inline constexpr float f_from_u(base u)
        {
            return (float)u;
        }

        inline constexpr float f_from_m(base m)
        {
            return (float)m / f_shift_half;
        }

        inline constexpr double d_from_e(extended e)
        {
            return (double)e / f_shift;
        }

        inline constexpr double d_from_u(base u)
        {
            return (double)u;
        }

        inline constexpr double d_from_m(base m)
        {
            return (double)m / f_shift_half;
        }

        inline constexpr extended e_from_f(float f)
        {
            return (extended)(f * f_shift);
        }

        inline constexpr extended e_from_d(double d)
        {
            return (extended)(d * f_shift);
        }

        inline constexpr base u_from_f(float f)
        {
            return (base)f;
        }

        inline constexpr base u_from_d(double d)
        {
            return (base)d;
        }

        inline constexpr base m_from_f(float f)
        {
            return (base)(f * f_shift_half);
        }

        inline constexpr base m_from_d(double d)
        {
            return (base)(d * f_shift_half);
        }

        static const base b_max = ((ubase)1 << (width - 1)) - 1;
        static const base b_min = -(b_max)-1;
        static const ubase ab_min = 0;
        static const ubase ab_max = (base)-1;
        static const extended e_max = ((uextended)1 << (width * 2 - 1)) - 1;
        static const extended e_min = -(e_max)-1;
        static const uextended ae_max = (extended)-1;
        static const uextended ae_min = 0;
        static const ubase b_sign_mask = (ubase)1 << (width - 1);
        static const ubase b_bits_mask = b_max;
        static const uextended e_sign_mask = (uextended)1 << (width * 2 - 1);
        static const uextended e_bits_mask = ((uextended)1 << (width * 2 - 1)) - 1;
        static const base m_max = ((base)1 << (width / 2 - 1)) - 1;
        static const base m_min = -((base)1 << (width / 2 - 1));
        static const ubase am_max = ((ubase)1 << (width / 2)) - 1;
        static const ubase am_min = 0;
        static const extended e_one = e_from_f(1.0f);
        static const base m_one = m_from_f(1.0f);
        static const base u_one = u_from_f(1.0f);
        static const float e_epsilon = f_from_e(1);
        static const float u_epsilon = f_from_u(1);
        static const float m_epsilon = f_from_m(1);
        static const float l_epsilon = f_from_l(1);
        static const float al_epsilon = f_from_al(1);

        namespace custom
        {
            union upper_parts
            {
                struct
                {
                    uhalf h2, h3;
                };
                struct
                {
                    ubase u_no_sign : (width - 1), u_sign : 1;
                };
                ubase au;
                base u;
            };

            union middle_parts
            {
                struct
                {
                    uhalf h1, h2;
                };
                struct
                {
                    ubase m_no_sign : (width - 1), m_sign : 1;
                };
                ubase am;
                base m;
            };

            union lower_parts
            {
                struct
                {
                    uhalf h0, h1;
                };
                struct
                {
                    ubase l_no_sign : (width - 1), l_sign : 1;
                };
                base l;
                ubase al;
            };

            union extended_parts
            {
                struct
                {
                    uhalf h0, h1, h2, h3;
                };
                struct
                {
                    ubase l_no_sign : (width - 1), l_sign : 1, u_no_sign : (width - 1), u_sign : 1;
                };
                struct
                {
                    ubase alpad : (width / 2), am : (width), aupad : (width / 2);
                };
                struct
                {
                    ubase lpad : (width / 2), m_no_sign : (width - 1), m_sign : 1, upad : (width / 2);
                };
                struct
                {
                    ubase al, au;
                };
                struct
                {
                    uextended e_no_sign : (width * 2 - 1), e_sign : 1;
                };
                uextended ae;
                extended e = 0;
            };

            // addition and subtraction can be done just using the x_from_y functions

            // multiplication
            // using half bases, the multiplication of two extended is defined as follows:
            // a0 b0 + a0 b1 x + a0 b2 x^2 + a0 b3 x^3 + a1 b0 x + a1 b1 x^2 + a1 b2 x^3 + a1 b3 x^4 + a2 b0 x^2 + a2 b1 x^3 + a2 b2 x^4 + a2 b3 x^5 + a3 b0 x^3 + a3 b1 x^4 + a3 b2 x^5 + a3 b3 x^6
            //(a_0 b_0) +
            //(a_0 b_1 + a_1 b_0) x +
            //(a_0 b_2 + a_1 b_1 + a_2 b_0) x^2 +
            //(a_0 b_3 + a_1 b_2 + a_2 b_1 + a_3 b_0) x^3 +
            //(a_1 b_3 + a_2 b_2 + a_3 b_1) x^4 +
            //(a_2 b_3 + a_3 b_2) x^5 +
            //(a_3 b_3) x^6

            inline constexpr ubase block_0(ubase a0, ubase b0)
            {
                return a0 * b0;
            }

            inline constexpr ubase block_1(ubase a0, ubase a1, ubase b0, ubase b1)
            {
                return a0 * b1 + a1 * b0;
            }

            inline constexpr ubase block_2(ubase a0, ubase a1, ubase a2, ubase b0, ubase b1, ubase b2)
            {
                return a0 * b2 + a1 * b1 + a2 * b0;
            }

            inline constexpr ubase block_3(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
            {
                return a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0;
            }

            inline constexpr ubase block_4(ubase a1, ubase a2, ubase a3, ubase b1, ubase b2, ubase b3)
            {
                return a1 * b3 + a2 * b2 + a3 * b1;
            }

            inline constexpr ubase block_5(ubase a2, ubase a3, ubase b2, ubase b3)
            {
                return a2 * b3 + a3 * b2;
            }

            inline constexpr ubase block_6(ubase a3, ubase b3)
            {
                return a3 * b3;
            }

            inline constexpr uextended e_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
            {
                // const ubase blk_0 = block_0(a0, b0);
                // const ubase blk_1 = block_1(a0, a1, b0, b1);
                const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
                const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
                const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
                const ubase blk_5 = block_5(a2, a3, b2, b3);
                // const ubase blk_6 = block_6(a3, b3);

                return (((((((uextended)blk_5 << width / 2) + blk_4) << width / 2) + blk_3) << width / 2) + blk_2) & e_bits_mask;
            }

            inline constexpr ubase u_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
            {
                // const ubase blk_0 = block_0(a0, b0);
                // const ubase blk_1 = block_1(a0, a1, b0, b1);
                // const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
                // const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
                const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
                const ubase blk_5 = block_5(a2, a3, b2, b3);
                // const ubase blk_6 = block_6(a3, b3);

                return ((blk_5 << width / 2) + blk_4) & b_bits_mask;
            }

            inline constexpr ubase m_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
            {
                // const ubase blk_0 = block_0(a0, b0);
                // const ubase blk_1 = block_1(a0, a1, b0, b1);
                // const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
                const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
                const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
                // const ubase blk_5 = block_5(a2, a3, b2, b3);
                // const ubase blk_6 = block_6(a3, b3);

                return ((blk_4 << width / 2) + blk_3) & b_bits_mask;
            }

            inline constexpr ubase al_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
            { // CAREFUL FOR THE SHIFT
                // const ubase blk_0 = block_0(a0, b0);
                // const ubase blk_1 = block_1(a0, a1, b0, b1);
                const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
                const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
                // const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
                // const ubase blk_5 = block_5(a2, a3, b2, b3);
                // const ubase blk_6 = block_6(a3, b3);

                return (((blk_3 << width / 2) + blk_2));
            }

            inline constexpr ubase l_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
            { // CAREFUL FOR THE SHIFT
                // const ubase blk_0 = block_0(a0, b0);
                // const ubase blk_1 = block_1(a0, a1, b0, b1);
                const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
                const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
                // const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
                // const ubase blk_5 = block_5(a2, a3, b2, b3);
                // const ubase blk_6 = block_6(a3, b3);

                return (((blk_3 << width / 2) + blk_2) / 2) & b_bits_mask;
            }

// disable narrowing conversion warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

            inline constexpr extended e_mul_ee(extended e1, extended e2)
            {
                return e1 * e2;

                const extended_parts a = {.e = abs(e1)};
                const extended_parts b = {.e = abs(e2)};

                return e_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_uu(base u1, base u2)
            {
                return (extended)u1 * u2;

                // const upper_parts a = { .au = abs(u1) };
                // const upper_parts b = { .au = abs(u2) };

                // return e_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_um(base u, base m)
            {
                const upper_parts a = {.au = abs(u)};
                const middle_parts b = {.am = abs(m)};

                return e_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_ul(base u, base l)
            { // CAREFUL FOR THE SHIFT
                const upper_parts a = {.au = abs(u)};
                const lower_parts b = {.al = abs(l) * 2};

                return e_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_ual(base u, ubase al)
            { // CAREFUL FOR THE SHIFT
                const upper_parts a = {.au = abs(u)};
                const lower_parts b = {.al = al};

                return e_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_mm(base m1, base m2)
            {
                const middle_parts a = {.am = abs(m1)};
                const middle_parts b = {.am = abs(m2)};

                return e_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_ml(base m, base l)
            { // CAREFUL FOR THE SHIFT
                const middle_parts a = {.am = abs(m)};
                const lower_parts b = {.al = abs(l) * 2};

                return e_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_mal(base m, ubase al)
            { // CAREFUL FOR THE SHIFT
                const middle_parts a = {.am = abs(m)};
                const lower_parts b = {.al = al};

                return e_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_ll(base l1, base l2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = abs(l1) * 2};
                const lower_parts b = {.al = abs(l2) * 2};

                return e_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_lal(base l1, ubase al2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = abs(l1) * 2};
                const lower_parts b = {.al = al2};

                return e_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ? -1 : 1);
            }

            inline constexpr extended e_mul_alal(ubase al1, ubase al2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = al1};
                const lower_parts b = {.al = al2};

                return e_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0);
            }

            inline constexpr base u_mul_ee(base e1, base e2)
            {
                const extended_parts a = {.e = abs(e1)};
                const extended_parts b = {.e = abs(e2)};

                return u_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_uu(base u1, base u2)
            {
                const upper_parts a = {.au = abs(u1)};
                const upper_parts b = {.au = abs(u2)};

                return u_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_um(base u, base m)
            {
                const upper_parts a = {.au = abs(u)};
                const middle_parts b = {.am = abs(m)};

                return u_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_ul(base u, base l)
            { // CAREFUL FOR THE SHIFT
                const upper_parts a = {.au = abs(u)};
                const lower_parts b = {.al = abs(l) * 2};

                return u_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_ual(base u, ubase al)
            { // CAREFUL FOR THE SHIFT
                const upper_parts a = {.au = abs(u)};
                const lower_parts b = {.al = al};

                return u_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_mm(base m1, base m2)
            {
                const middle_parts a = {.am = abs(m1)};
                const middle_parts b = {.am = abs(m2)};

                return u_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_ml(base m, base l)
            { // CAREFUL FOR THE SHIFT
                const middle_parts a = {.am = abs(m)};
                const lower_parts b = {.al = abs(l) * 2};

                return u_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_mal(base m, ubase al)
            { // CAREFUL FOR THE SHIFT
                const middle_parts a = {.am = abs(m)};
                const lower_parts b = {.al = al};

                return u_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_ll(base l1, base l2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = abs(l1) * 2};
                const lower_parts b = {.al = abs(l2) * 2};

                return u_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_lal(base l1, ubase al2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = abs(l1) * 2};
                const lower_parts b = {.al = al2};

                return u_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ? -1 : 1);
            }

            inline constexpr base u_mul_alal(ubase al1, ubase al2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = al1};
                const lower_parts b = {.al = al2};

                return u_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0);
            }

            inline constexpr base m_mul_ee(base e1, base e2)
            {
                const extended_parts a = {.ae = abs(e1)};
                const extended_parts b = {.ae = abs(e2)};

                return m_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_uu(base u1, base u2)
            {
                const upper_parts a = {.au = abs(u1)};
                const upper_parts b = {.au = abs(u2)};

                return m_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_um(base u, base m)
            {
                const upper_parts a = {.au = abs(u)};
                const middle_parts b = {.am = abs(m)};

                return m_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_ul(base u, base l)
            { // CAREFUL FOR THE SHIFT
                const upper_parts a = {.au = abs(u)};
                const lower_parts b = {.al = abs(l) * 2};

                return m_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_ual(base u, ubase al)
            { // CAREFUL FOR THE SHIFT
                const upper_parts a = {.au = abs(u)};
                const lower_parts b = {.al = al};

                return m_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_mm(base m1, base m2)
            {
                const middle_parts a = {.am = abs(m1)};
                const middle_parts b = {.am = abs(m2)};

                return m_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_ml(base m, base l)
            { // CAREFUL FOR THE SHIFT
                const middle_parts a = {.am = abs(m)};
                const lower_parts b = {.al = abs(l) * 2};

                return m_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_mal(base m, ubase al)
            { // CAREFUL FOR THE SHIFT
                const middle_parts a = {.am = abs(m)};
                const lower_parts b = {.al = al};

                return m_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_ll(base l1, base l2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = abs(l1) * 2};
                const lower_parts b = {.al = abs(l2) * 2};

                return m_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_lal(base l1, ubase al2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = abs(l1) * 2};
                const lower_parts b = {.al = al2};

                return m_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ? -1 : 1);
            }

            inline constexpr base m_mul_alal(ubase al1, ubase al2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = al1};
                const lower_parts b = {.al = al2};

                return m_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0);
            }

            inline constexpr base l_mul_ee(base e1, base e2)
            {
                const extended_parts a = {.ae = abs(e1)};
                const extended_parts b = {.ae = abs(e2)};

                return l_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
            }

            inline constexpr base l_mul_uu(base u1, base u2)
            {
                const upper_parts a = {.au = abs(u1)};
                const upper_parts b = {.au = abs(u2)};

                return l_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
            }

            inline constexpr base l_mul_um(base u, base m)
            {
                const upper_parts a = {.au = abs(u)};
                const middle_parts b = {.am = abs(m)};

                return l_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
            }

            inline constexpr base l_mul_ul(base u, base l)
            { // CAREFUL FOR THE SHIFT
                const upper_parts a = {.au = abs(u)};
                const lower_parts b = {.al = abs(l) * 2};

                return l_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
            }

            inline constexpr base l_mul_ual(base u, ubase al)
            { // CAREFUL FOR THE SHIFT
                const upper_parts a = {.au = abs(u)};
                const lower_parts b = {.al = al};

                return l_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ? -1 : 1);
            }

            inline constexpr base l_mul_mm(base m1, base m2)
            {
                const middle_parts a = {.am = abs(m1)};
                const middle_parts b = {.am = abs(m2)};

                return l_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
            }

            inline constexpr base l_mul_ml(base m, base l)
            { // CAREFUL FOR THE SHIFT
                const middle_parts a = {.am = abs(m)};
                const lower_parts b = {.al = abs(l) * 2};

                return l_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
            }

            inline constexpr base l_mul_mal(base m, ubase al)
            { // CAREFUL FOR THE SHIFT
                const middle_parts a = {.am = abs(m)};
                const lower_parts b = {.al = al};

                return l_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ? -1 : 1);
            }

            inline constexpr ubase al_mul_mal(base m, ubase al)
            { // CAREFUL FOR THE SHIFT
                const middle_parts a = {.am = abs(m)};
                const lower_parts b = {.al = al};

                return al_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0);
            }

            inline constexpr base l_mul_ll(base l1, base l2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = abs(l1) * 2};
                const lower_parts b = {.al = abs(l2) * 2};

                return l_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
            }

            inline constexpr base l_mul_lal(base l1, ubase al2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = abs(l1) * 2};
                const lower_parts b = {.al = al2};

                return l_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ? -1 : 1);
            }

            inline constexpr base l_mul_alal(ubase al1, ubase al2)
            { // CAREFUL FOR THE SHIFT
                const lower_parts a = {.al = al1};
                const lower_parts b = {.al = al2};

                return l_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0);
            }
// enable narrowing conversion warnings
#pragma GCC diagnostic pop
        } // namespace custom

        namespace native
        {

            #define __FPM_MULT_TYPE_e extended
            #define __FPM_MULT_TYPE_ae uextended
            #define __FPM_MULT_TYPE_u base
            #define __FPM_MULT_TYPE_au ubase
            #define __FPM_MULT_TYPE_m base
            #define __FPM_MULT_TYPE_am ubase
            #define __FPM_MULT_TYPE_l base
            #define __FPM_MULT_TYPE_al ubase

            // e starts on bit n 0
            // u starts on bit n w
            // l starts on bit n 1
            // al strts on bit n 0
            // m starts on bit n w/2

            #define __FPM_MULT_STARTBIT_e 0
            #define __FPM_MULT_STARTBIT_ae 0
            #define __FPM_MULT_STARTBIT_u width
            #define __FPM_MULT_STARTBIT_au width
            #define __FPM_MULT_STARTBIT_m (width/2)
            #define __FPM_MULT_STARTBIT_am (width/2)
            #define __FPM_MULT_STARTBIT_l 1
            #define __FPM_MULT_STARTBIT_al 0

// disable shift overflow warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-count-overflow"
            template<int bits>
            inline constexpr extended __FPM_MULT_SHIFT_IMPL(extended n)
            {
                if (bits >= width * 2)
                    return 0;
                else if (bits <= -width * 2)
                    return 0;
                if (bits > 0)
                    return (n << bits);
                else
                    return (n >> (-bits));
            }
#pragma GCC diagnostic pop
            
            #define __FPM_MULT_SHIFT(n, bits) __FPM_MULT_SHIFT_IMPL<bits>(n)
            #define __FPM_MULT_COMP(_r_, _a_, _b_) ( \
                +__FPM_MULT_STARTBIT_##_a_ +__FPM_MULT_STARTBIT_##_b_ -__FPM_MULT_STARTBIT_##_r_ - width\
                )

            #define __FPM_MULT(_r_, _a_, _b_) \
                inline constexpr __FPM_MULT_TYPE_##_r_ _r_##_mul_##_a_##_b_(__FPM_MULT_TYPE_##_a_ a, __FPM_MULT_TYPE_##_b_ b) \
                { \
                    return (__FPM_MULT_TYPE_##_r_) __FPM_MULT_SHIFT((extended)a * b, __FPM_MULT_COMP(_r_, _a_, _b_)); \
                }

            __FPM_MULT(e, e, e)
            __FPM_MULT(e, e, u)
            __FPM_MULT(e, e, m)
            __FPM_MULT(e, e, l)
            __FPM_MULT(e, e, al)
            __FPM_MULT(e, u, e)
            __FPM_MULT(e, u, u)
            __FPM_MULT(e, u, m)
            __FPM_MULT(e, u, l)
            __FPM_MULT(e, u, al)
            __FPM_MULT(e, m, e)
            __FPM_MULT(e, m, u)
            __FPM_MULT(e, m, m)
            __FPM_MULT(e, m, l)
            __FPM_MULT(e, m, al)
            __FPM_MULT(e, l, e)
            __FPM_MULT(e, l, u)
            __FPM_MULT(e, l, m)
            __FPM_MULT(e, l, l)
            __FPM_MULT(e, l, al)
            __FPM_MULT(e, al, e)
            __FPM_MULT(e, al, u)
            __FPM_MULT(e, al, m)
            __FPM_MULT(e, al, l)
            __FPM_MULT(e, al, al)

            __FPM_MULT(u, e, e)
            __FPM_MULT(u, e, u)
            __FPM_MULT(u, e, m)
            __FPM_MULT(u, e, l)
            __FPM_MULT(u, e, al)
            __FPM_MULT(u, u, e)
            __FPM_MULT(u, u, u)
            __FPM_MULT(u, u, m)
            __FPM_MULT(u, u, l)
            __FPM_MULT(u, u, al)
            __FPM_MULT(u, m, e)
            __FPM_MULT(u, m, u)
            __FPM_MULT(u, m, m)
            __FPM_MULT(u, m, l)
            __FPM_MULT(u, m, al)
            __FPM_MULT(u, l, e)
            __FPM_MULT(u, l, u)
            __FPM_MULT(u, l, m)
            __FPM_MULT(u, l, l)
            __FPM_MULT(u, l, al)
            __FPM_MULT(u, al, e)
            __FPM_MULT(u, al, u)
            __FPM_MULT(u, al, m)
            __FPM_MULT(u, al, l)
            __FPM_MULT(u, al, al)

            __FPM_MULT(m, e, e)
            __FPM_MULT(m, e, u)
            __FPM_MULT(m, e, m)
            __FPM_MULT(m, e, l)
            __FPM_MULT(m, e, al)
            __FPM_MULT(m, u, e)
            __FPM_MULT(m, u, u)
            __FPM_MULT(m, u, m)
            __FPM_MULT(m, u, l)
            __FPM_MULT(m, u, al)
            __FPM_MULT(m, m, e)
            __FPM_MULT(m, m, u)
            __FPM_MULT(m, m, m)
            __FPM_MULT(m, m, l)
            __FPM_MULT(m, m, al)
            __FPM_MULT(m, l, e)
            __FPM_MULT(m, l, u)
            __FPM_MULT(m, l, m)
            __FPM_MULT(m, l, l)
            __FPM_MULT(m, l, al)
            __FPM_MULT(m, al, e)
            __FPM_MULT(m, al, u)
            __FPM_MULT(m, al, m)
            __FPM_MULT(m, al, l)
            __FPM_MULT(m, al, al)

            __FPM_MULT(l, e, e)
            __FPM_MULT(l, e, u)
            __FPM_MULT(l, e, m)
            __FPM_MULT(l, e, l)
            __FPM_MULT(l, e, al)
            __FPM_MULT(l, u, e)
            __FPM_MULT(l, u, u)
            __FPM_MULT(l, u, m)
            __FPM_MULT(l, u, l)
            __FPM_MULT(l, u, al)
            __FPM_MULT(l, m, e)
            __FPM_MULT(l, m, u)
            __FPM_MULT(l, m, m)
            __FPM_MULT(l, m, l)
            __FPM_MULT(l, m, al)
            __FPM_MULT(l, l, e)
            __FPM_MULT(l, l, u)
            __FPM_MULT(l, l, m)
            __FPM_MULT(l, l, l)
            __FPM_MULT(l, l, al)
            __FPM_MULT(l, al, e)
            __FPM_MULT(l, al, u)
            __FPM_MULT(l, al, m)
            __FPM_MULT(l, al, l)
            __FPM_MULT(l, al, al)

            __FPM_MULT(al, e, e)
            __FPM_MULT(al, e, u)
            __FPM_MULT(al, e, m)
            __FPM_MULT(al, e, l)
            __FPM_MULT(al, e, al)
            __FPM_MULT(al, u, e)
            __FPM_MULT(al, u, u)
            __FPM_MULT(al, u, m)
            __FPM_MULT(al, u, l)
            __FPM_MULT(al, u, al)
            __FPM_MULT(al, m, e)
            __FPM_MULT(al, m, u)
            __FPM_MULT(al, m, m)
            __FPM_MULT(al, m, l)
            __FPM_MULT(al, m, al)
            __FPM_MULT(al, l, e)
            __FPM_MULT(al, l, u)
            __FPM_MULT(al, l, m)
            __FPM_MULT(al, l, l)
            __FPM_MULT(al, l, al)
            __FPM_MULT(al, al, e)
            __FPM_MULT(al, al, u)
            __FPM_MULT(al, al, m)
            __FPM_MULT(al, al, l)
            __FPM_MULT(al, al, al)

        } // namespace native

#ifdef FPM_NATIVE_MULT
    using namespace native;
#else
    using namespace custom;
#endif

    }
}