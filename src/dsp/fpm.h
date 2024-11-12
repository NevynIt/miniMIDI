#pragma once

#include <cstdint>

#ifndef FPM_WIDTH
#define FPM_WIDTH 16
#endif

namespace dsp {
    namespace fpm {

        #if FPM_WIDTH == 16
            static const size_t width = 16;
            using half = int8_t;
            using base = int16_t;
            using extended = int32_t;
            using uhalf = uint8_t;
            using ubase = uint16_t;
            using uextended = uint32_t;
        #elif FPM_WIDTH == 32
            static const size_t width = 32;
            using half = int16_t;
            using base = int32_t;
            using extended = int64_t;
            using uhalf = uint16_t;
            using ubase = uint32_t;
            using uextended = uint64_t;
        #else
            #error "Unsupported FPM_WIDTH"
        #endif 

        union upper_parts {
            struct {
                uhalf h2, h3;
            };
            ubase b;
        };

        union middle_parts {
            struct {
                uhalf h1, h2;
            };
            ubase b;
        };

        union lower_parts {
            struct {
                uhalf h0, h1;
            };
            ubase b;
        };

        union extended_parts {
            struct {
                uhalf h0, h1, h2, h3;
            };
            uextended e;
        };

        //think of a fixed point integer of type "extended", with FPM_WIDTH bits on each side of the decimal point (1 of them being the sign bit)
        //base variables with prefix e represent the extended variable
        //base variables with prefix u represent the upper half of the extended variable --> the integer part
        //base variables with prefix l represent the lower half of the extended variable --> the fractional part
        //base variables with prefix m represent the middle part of the extended variable --> another fixed point number with
        //      FPM_WIDTH/2 bits on each side of the decimal point
        
        //math functions are defined for mixed arguments and have the following naming convention:
        //      <output type>_<operator>_<input types>

        // conversion
        // functions returning extended
        inline constexpr extended e_from_u(base u){
            return (extended)u << FPM_WIDTH;
        }

        inline constexpr extended e_from_l(base l){
            return l;
        }

        inline constexpr extended e_from_m(base m){
            return (extended)m << (FPM_WIDTH/2);
        }

        // functions returning upper
        inline constexpr base u_from_e(extended e){
            return e >> FPM_WIDTH;
        }

        inline constexpr base u_from_l(base l){
            return 0;
        }

        inline constexpr base u_from_m(base m){
            return m >> (FPM_WIDTH/2);
        }

        // functions returning lower
        inline constexpr base l_from_e(extended e){
            return static_cast<base>(e);
        }

        inline constexpr base l_from_u(base u){
            return 0;
        }

        inline constexpr base l_from_m(base m){
            return m  << (FPM_WIDTH/2);
        }

        inline constexpr float f_from_e(extended e){
            return (float)e / ((base)1 << (FPM_WIDTH-1)) / 2;
        } 

        inline constexpr float f_from_u(base u){
            return (float)u;
        }   

        inline constexpr float f_from_l(base l){
            return (float)l / ((base)1 << (FPM_WIDTH-1)) / 2;
        }

        inline constexpr float f_from_m(base m){
            return (float)m / ((base)1 << (FPM_WIDTH/2));
        }

        inline constexpr double d_from_e(extended e){
            return (double)e / ((base)1 << (FPM_WIDTH-1)) / 2;
        }

        inline constexpr double d_from_u(base u){
            return (double)u;
        }

        inline constexpr double d_from_l(base l){
            return (double)l / ((base)1 << (FPM_WIDTH-1)) / 2;
        }

        inline constexpr double d_from_m(base m){
            return (double)m / ((base)1 << (FPM_WIDTH/2));
        }

        inline constexpr extended e_from_f(float f){
            return (extended)(f * ((base)1 << (FPM_WIDTH-1)) * 2);
        }

        inline constexpr extended e_from_d(double d){
            return (extended)(d * ((base)1 << (FPM_WIDTH-1)) * 2);
        }

        inline constexpr base u_from_f(float f){
            return (base)f;
        }

        inline constexpr base u_from_d(double d){
            return (base)d;
        }

        inline constexpr base l_from_f(float f){
            return (base)(f * ((base)1 << (FPM_WIDTH-1)) * 2);
        }

        inline constexpr base l_from_d(double d){
            return (base)(d * ((base)1 << (FPM_WIDTH-1)) * 2);
        }

        inline constexpr base m_from_f(float f){
            return (base)(f * ((base)1 << (FPM_WIDTH/2)));
        }

        inline constexpr base m_from_d(double d){
            return (base)(d * ((base)1 << (FPM_WIDTH/2)));
        }

        inline constexpr uint8_t abs(int8_t x){
            return x < 0 ? -x : x;
        }

        inline constexpr uint16_t abs(int16_t x){
            return x < 0 ? -x : x;
        }

        inline constexpr uint32_t abs(int32_t x){
            return x < 0 ? -x : x;
        }

        inline constexpr uint64_t abs(int64_t x){
            return x < 0 ? -x : x;
        }
        
        static const base m_one = m_from_f(1.0f);
        static const base u_one = u_from_f(1.0f);
        static const base l_one = l_from_f(1.0f);

        //addition and subtraction can be done just using the x_from_y functions

        //multiplication
        //using half bases, the multiplication of two extended is defined as follows:
        //a0 b0 + a0 b1 x + a0 b2 x^2 + a0 b3 x^3 + a1 b0 x + a1 b1 x^2 + a1 b2 x^3 + a1 b3 x^4 + a2 b0 x^2 + a2 b1 x^3 + a2 b2 x^4 + a2 b3 x^5 + a3 b0 x^3 + a3 b1 x^4 + a3 b2 x^5 + a3 b3 x^6
        //(a_0 b_0) + 
        //(a_0 b_1 + a_1 b_0) x + 
        //(a_0 b_2 + a_1 b_1 + a_2 b_0) x^2 + 
        //(a_0 b_3 + a_1 b_2 + a_2 b_1 + a_3 b_0) x^3 +
        //(a_1 b_3 + a_2 b_2 + a_3 b_1) x^4 +
        //(a_2 b_3 + a_3 b_2) x^5 +
        //(a_3 b_3) x^6 //not used

        inline constexpr ubase block_0(ubase a0, ubase b0){
            return a0 * b0;
        }

        inline constexpr ubase block_1(ubase a0, ubase a1, ubase b0, ubase b1){
            return a0 * b1 + a1 * b0;
        }

        inline constexpr ubase block_2(ubase a0, ubase a1, ubase a2, ubase b0, ubase b1, ubase b2){
            return a0 * b2 + a1 * b1 + a2 * b0;
        }

        inline constexpr ubase block_3(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3){
            return a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0;
        }

        inline constexpr ubase block_4(ubase a1, ubase a2, ubase a3, ubase b1, ubase b2, ubase b3){
            return a1 * b3 + a2 * b2 + a3 * b1;
        }

        inline constexpr ubase block_5(ubase a2, ubase a3, ubase b2, ubase b3){
            return a2 * b3 + a3 * b2;
        }

        inline constexpr ubase block_6(ubase a3, ubase b3){
            return a3 * b3;
        }

        inline constexpr uextended e_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3){
            const ubase blk_0 = block_0(a0, b0);
            const ubase blk_1 = block_1(a0, a1, b0, b1);
            const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
            const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
            const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
            const ubase blk_5 = block_5(a2, a3, b2, b3);
            const ubase blk_6 = block_6(a3, b3);

            return (((((((uextended)blk_5 << FPM_WIDTH/2) + blk_4) << FPM_WIDTH/2) + blk_3) << FPM_WIDTH/2) + blk_2) & ~((uextended)1 << ((FPM_WIDTH*2) - 1));
        }

        inline constexpr ubase u_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3){
            const ubase blk_0 = block_0(a0, b0);
            const ubase blk_1 = block_1(a0, a1, b0, b1);
            const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
            const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
            const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
            const ubase blk_5 = block_5(a2, a3, b2, b3);
            const ubase blk_6 = block_6(a3, b3);

            return ((blk_5 << FPM_WIDTH/2) + blk_4) & ~((ubase)1 << (FPM_WIDTH - 1));
        }


        inline constexpr ubase m_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3){
            const ubase blk_0 = block_0(a0, b0);
            const ubase blk_1 = block_1(a0, a1, b0, b1);
            const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
            const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
            const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
            const ubase blk_5 = block_5(a2, a3, b2, b3);
            const ubase blk_6 = block_6(a3, b3);

            return ((blk_4 << FPM_WIDTH/2) + blk_3) & ~((ubase)1 << (FPM_WIDTH - 1));
        }


        inline constexpr ubase l_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3){
            const ubase blk_0 = block_0(a0, b0);
            const ubase blk_1 = block_1(a0, a1, b0, b1);
            const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
            const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
            const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
            const ubase blk_5 = block_5(a2, a3, b2, b3);
            const ubase blk_6 = block_6(a3, b3);

            return ((blk_3 << FPM_WIDTH/2) + blk_2) & ~((ubase)1 << (FPM_WIDTH - 1));
        }

        inline constexpr extended e_mul_ee(extended e1, extended e2){
            const extended_parts a = { .e = abs(e1) };
            const extended_parts b = { .e = abs(e2) };

            return e_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
        }
        
        inline constexpr extended e_mul_uu(base u1, base u2){
            const upper_parts a = { .b = abs(u1) };
            const upper_parts b = { .b = abs(u2) };

            return e_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
        }
        
        inline constexpr extended e_mul_um(base u, base m){
            const upper_parts a = { .b = abs(u) };
            const middle_parts b = { .b = abs(m) };

            return e_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
        }
        
        inline constexpr extended e_mul_ul(base u, base l){
            const upper_parts a = { .b = abs(u) };
            const lower_parts b = { .b = abs(l) };

            return e_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
        }

        inline constexpr extended e_mul_mm(base m1, base m2){
            const middle_parts a = { .b = abs(m1) };
            const middle_parts b = { .b = abs(m2) };

            return e_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
        }

        inline constexpr extended e_mul_ml(base m, base l){
            const middle_parts a = { .b = abs(m) };
            const lower_parts b = { .b = abs(l) };

            return e_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
        }

        inline constexpr extended e_mul_ll(base l1, base l2){
            const lower_parts a = { .b = abs(l1) };
            const lower_parts b = { .b = abs(l2) };

            return e_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
        }

        inline constexpr base u_mul_ee(base e1, base e2){
            const extended_parts a = { .e = abs(e1) };
            const extended_parts b = { .e = abs(e2) };

            return u_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
        }
        
        inline constexpr base u_mul_uu(base u1, base u2){
            const upper_parts a = { .b = abs(u1) };
            const upper_parts b = { .b = abs(u2) };

            return u_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
        }
        
        inline constexpr base u_mul_um(base u, base m){
            const upper_parts a = { .b = abs(u) };
            const middle_parts b = { .b = abs(m) };

            return u_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
        }
        
        inline constexpr base u_mul_ul(base u, base l){
            const upper_parts a = { .b = abs(u) };
            const lower_parts b = { .b = abs(l) };

            return u_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
        }

        inline constexpr base u_mul_mm(base m1, base m2){
            const middle_parts a = { .b = abs(m1) };
            const middle_parts b = { .b = abs(m2) };

            return u_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
        }

        inline constexpr base u_mul_ml(base m, base l){
            const middle_parts a = { .b = abs(m) };
            const lower_parts b = { .b = abs(l) };

            return u_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
        }

        inline constexpr base u_mul_ll(base l1, base l2){
            const lower_parts a = { .b = abs(l1) };
            const lower_parts b = { .b = abs(l2) };

            return u_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
        }

        inline constexpr base m_mul_ee(base e1, base e2){
            const extended_parts a = { .e = abs(e1) };
            const extended_parts b = { .e = abs(e2) };

            return m_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
        }
        
        inline constexpr base m_mul_uu(base u1, base u2){
            const upper_parts a = { .b = abs(u1) };
            const upper_parts b = { .b = abs(u2) };

            return m_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
        }
        
        inline constexpr base m_mul_um(base u, base m){
            const upper_parts a = { .b = abs(u) };
            const middle_parts b = { .b = abs(m) };

            return m_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
        }
        
        inline constexpr base m_mul_ul(base u, base l){
            const upper_parts a = { .b = abs(u) };
            const lower_parts b = { .b = abs(l) };

            return m_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
        }

        inline constexpr base m_mul_mm(base m1, base m2){
            const middle_parts a = { .b = abs(m1) };
            const middle_parts b = { .b = abs(m2) };

            return m_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
        }

        inline constexpr base m_mul_ml(base m, base l){
            const middle_parts a = { .b = abs(m) };
            const lower_parts b = { .b = abs(l) };

            return m_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
        }

        inline constexpr base m_mul_ll(base l1, base l2){
            const lower_parts a = { .b = abs(l1) };
            const lower_parts b = { .b = abs(l2) };

            return m_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
        }

        inline constexpr base l_mul_ee(base e1, base e2){
            const extended_parts a = { .e = abs(e1) };
            const extended_parts b = { .e = abs(e2) };

            return l_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
        }
        
        inline constexpr base l_mul_uu(base u1, base u2){
            const upper_parts a = { .b = abs(u1) };
            const upper_parts b = { .b = abs(u2) };

            return l_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
        }
        
        inline constexpr base l_mul_um(base u, base m){
            const upper_parts a = { .b = abs(u) };
            const middle_parts b = { .b = abs(m) };

            return l_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
        }
        
        inline constexpr base l_mul_ul(base u, base l){
            const upper_parts a = { .b = abs(u) };
            const lower_parts b = { .b = abs(l) };

            return l_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
        }

        inline constexpr base l_mul_mm(base m1, base m2){
            const middle_parts a = { .b = abs(m1) };
            const middle_parts b = { .b = abs(m2) };

            return l_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
        }

        inline constexpr base l_mul_ml(base m, base l){
            const middle_parts a = { .b = abs(m) };
            const lower_parts b = { .b = abs(l) };

            return l_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
        }

        inline constexpr base l_mul_ll(base l1, base l2){
            const lower_parts a = { .b = abs(l1) };
            const lower_parts b = { .b = abs(l2) };

            return l_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
        }
    }
}