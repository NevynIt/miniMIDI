#pragma once

#include <cstdint>
#include <cmath>

#ifndef FPM_WIDTH
#define FPM_WIDTH 16
#endif

// #define FPM_CUSTOM_MULT

namespace dsp
{
    namespace fpm
    {

#if FPM_WIDTH == 16
        using half = int8_t;
        using base = int16_t;
        using ext = int32_t;
        using uhalf = uint8_t;
        using ubase = uint16_t;
        using uext = uint32_t;
        static constexpr int8_t width = 16;

        #define FPM_EXTEXT
        using extext = int64_t;
        using uextext = uint64_t;
        
#elif FPM_WIDTH == 32
        using half = int16_t;
        using base = int32_t;
        using ext = int64_t;
        using uhalf = uint16_t;
        using ubase = uint32_t;
        using uext = uint64_t;
        static constexpr int8_t width = 32;
#else
#error "Unsupported width"
#endif

        // think of a fixed point integer of type "ext", with width bits on each side of the decimal point (1 of them being the sign bit)
        // base variables with prefix e represent the ext variable
        // base variables with prefix u represent the upper half of the ext variable --> the integer part
        // base variables with prefix l represent the lower half of the ext variable --> the fractional part
        // base variables with prefix m represent the middle part of the ext variable --> another fixed point number with
        //       width/2 bits on each side of the decimal point
        //  ext has the bits like this:
        //  <sign : 1><upper : width -1><lower : width -1><extra : 1>
        //  <sign : 1><pad : width/2><middle : width><pad : width/2-1>

        // math functions are defined for mixed arguments and have the following naming convention:
        //       <output type>_<operator>_<input types>

        using e_type = ext;     //double size balanced
        using ae_type = uext;   //double size balanced unsigned
        using u_type = base;    //full size integer
        using au_type = ubase;  //full size unsigned integer
        using m_type = base;    //full size balanced
        using am_type = ubase;  //full size balanced unsigned
        using l_type = base;    //full size fractional
        using al_type = ubase;  
        static constexpr int8_t e_start_bit = width;
        static constexpr int8_t ae_start_bit = width;
        static constexpr int8_t u_start_bit = (width*2);
        static constexpr int8_t au_start_bit = (width*2);
        static constexpr int8_t m_start_bit = (width + width/2);
        static constexpr int8_t am_start_bit = (width + width/2);
        static constexpr int8_t l_start_bit = (width + 1);
        static constexpr int8_t al_start_bit = width;
        static constexpr int8_t e_int_bits = (width-1);
        static constexpr int8_t ae_int_bits = width;
        static constexpr int8_t u_int_bits = (width-1);
        static constexpr int8_t au_int_bits = (width);
        static constexpr int8_t m_int_bits = (width/2-1);
        static constexpr int8_t am_int_bits = (width/2);
        static constexpr int8_t l_int_bits = 0;
        static constexpr int8_t al_int_bits = 0;
        static constexpr int8_t e_frac_bits = width;
        static constexpr int8_t ae_frac_bits = width;
        static constexpr int8_t u_frac_bits = 0;
        static constexpr int8_t au_frac_bits = 0;
        static constexpr int8_t m_frac_bits = (width/2);
        static constexpr int8_t am_frac_bits = (width/2);
        static constexpr int8_t l_frac_bits = (width - 1);
        static constexpr int8_t al_frac_bits = width;

        #ifdef FPM_EXTEXT
        using ee_type = extext;     //quadruple size balanced
        using aee_type = uextext;   //quadruple size balanced unsigned
        using eu_type = ext;        //double size integer
        using aeu_type = uext;      //double size unsigned integer
        using emu_type = ext;       //double size integer focused
        using aemu_type = uext;     //double size unsigned integer focused
        using em_type = ext;        //double size balanced
        using aem_type = uext;      //double size balanced unsigned
        using eml_type = ext;       //double size fractional focused
        using aeml_type = uext;     //double size unsigned fractional focused
        using el_type = ext;        //double size fractional
        using ael_type = uext;      //double size unsigned fractional
        static constexpr int8_t ee_start_bit = 0;
        static constexpr int8_t aee_start_bit = 0;
        static constexpr int8_t eu_start_bit = (width*2);
        static constexpr int8_t aeu_start_bit = (width*2);
        static constexpr int8_t emu_start_bit = (width + width/2);
        static constexpr int8_t aemu_start_bit = (width + width/2);
        static constexpr int8_t em_start_bit = (width);
        static constexpr int8_t aem_start_bit = (width);
        static constexpr int8_t eml_start_bit = (width/2);
        static constexpr int8_t aeml_start_bit = (width/2);
        static constexpr int8_t el_start_bit = 1;
        static constexpr int8_t ael_start_bit = 0;
        static constexpr int8_t ee_int_bits = (width*2-1);
        static constexpr int8_t aee_int_bits = (width*2);
        static constexpr int8_t eu_int_bits = (width*2-1);
        static constexpr int8_t aeu_int_bits = (width*2);
        static constexpr int8_t emu_int_bits = (width+width/2-1);
        static constexpr int8_t aemu_int_bits = (width+width/2);
        static constexpr int8_t em_int_bits = (width-1);
        static constexpr int8_t aem_int_bits = (width);
        static constexpr int8_t eml_int_bits = (width/2-1);
        static constexpr int8_t aeml_int_bits = (width/2);
        static constexpr int8_t el_int_bits = 0;
        static constexpr int8_t ael_int_bits = 0;
        static constexpr int8_t ee_frac_bits = (width*2);
        static constexpr int8_t aee_frac_bits = (width*2);
        static constexpr int8_t eu_frac_bits = 0;
        static constexpr int8_t aeu_frac_bits = 0;
        static constexpr int8_t emu_frac_bits = (width/2);
        static constexpr int8_t aemu_frac_bits = (width/2);
        static constexpr int8_t em_frac_bits = (width);
        static constexpr int8_t aem_frac_bits = (width);
        static constexpr int8_t eml_frac_bits = (width+width/2);
        static constexpr int8_t aeml_frac_bits = (width+width/2);
        static constexpr int8_t el_frac_bits = (width*2-1);
        static constexpr int8_t ael_frac_bits = (width*2);
        #endif

// disable shift overflow warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-count-overflow"
            template<typename T, int bits>
            inline constexpr T shift(T n)
            {
                if (abs(bits) >= sizeof(T) * 8)
                    return 0;
                if (bits > 0)
                    return (n << bits);
                else
                    return (n >> (-bits));
            }
#pragma GCC diagnostic pop
            
        // conversion
        #define __FPM_CONV(_r_, _w_, _a_) \
            inline constexpr _r_##_type _r_##_from_##_a_(_a_##_type a) \
            { \
                return (_r_##_type) shift<_w_##_type, _a_##_start_bit -_r_##_start_bit>(a); \
            }

        #include "fpm_conv.inc"

        #define __FPM_CONV_FLP(_w_, _t_) \
            inline constexpr float f_from_##_t_(_t_##_type a) \
            { \
                return (float)a / ((_w_##_type)1 << _t_##_frac_bits); \
            } \
            inline constexpr _t_##_type _t_##_from_f(float a) \
            { \
                return (_t_##_type) (a * ((_w_##_type)1 << _t_##_frac_bits)); \
            } \
            inline constexpr double d_from_##_t_(_t_##_type a) \
            { \
                return (double)a / ((_w_##_type)1 << _t_##_frac_bits); \
            } \
            inline constexpr _t_##_type _t_##_from_d(double a) \
            { \
                return (_t_##_type) (a * ((_w_##_type)1 << _t_##_frac_bits)); \
            }
        
        __FPM_CONV_FLP(ae, e)
        __FPM_CONV_FLP(ae, ae)
        __FPM_CONV_FLP(ae, u)
        __FPM_CONV_FLP(ae, au)
        __FPM_CONV_FLP(ae, m)
        __FPM_CONV_FLP(ae, am)
        __FPM_CONV_FLP(ae, l)
        __FPM_CONV_FLP(ae, al)

        #ifdef FPM_EXTEXT
        __FPM_CONV_FLP(ee, ee)
        __FPM_CONV_FLP(ee, aee)
        __FPM_CONV_FLP(e, eu)
        __FPM_CONV_FLP(e, aeu)
        __FPM_CONV_FLP(e, emu)
        __FPM_CONV_FLP(e, aemu)
        __FPM_CONV_FLP(e, em)
        __FPM_CONV_FLP(e, aem)
        __FPM_CONV_FLP(e, eml)
        __FPM_CONV_FLP(e, aeml)
        __FPM_CONV_FLP(ee, el)
        __FPM_CONV_FLP(ee, ael)
        #endif

        // native multiplication and division
        namespace native
        {
            #define __FPM_MUL(_w_, _r_, _a_, _b_) \
                inline constexpr _r_##_type _r_##_mul_##_a_##_b_(_a_##_type a, _b_##_type b) \
                { \
                    return (_r_##_type) shift<_w_##_type, _a_##_start_bit +_b_##_start_bit -_r_##_start_bit - width*2>((_w_##_type)a * b); \
                }

            #define __FPM_DIV(_w_, _r_, _a_, _b_) \
                inline constexpr _r_##_type _r_##_div_##_a_##_b_(_a_##_type a, _b_##_type b) \
                { \
                    return (_r_##_type) shift<_w_##_type, -_a_##_start_bit +_b_##_start_bit + (width*2) -_r_##_start_bit -8*((int)sizeof(_w_##_type)-(int)sizeof(_a_##_type))>(shift<_w_##_type,8*((int)sizeof(_w_##_type)-(int)sizeof(_a_##_type))>(a)/b); \
                }

            #include "fpm_mul.inc"
        } // namespace native

        #define __FPM_LIMITS(_t_) \
            static const _t_##_type _t_##_max = std::numeric_limits<_t_##_type>::max(); \
            static const _t_##_type _t_##_min = std::numeric_limits<_t_##_type>::min(); \
            static const float _t_##_epsilon = f_from_##_t_(1);

        __FPM_LIMITS(e)
        __FPM_LIMITS(ae)
        __FPM_LIMITS(u)
        __FPM_LIMITS(au)
        __FPM_LIMITS(m)
        __FPM_LIMITS(am)
        __FPM_LIMITS(l)
        __FPM_LIMITS(al)

        #ifdef FPM_EXTEXT
        __FPM_LIMITS(ee)
        __FPM_LIMITS(aee)
        __FPM_LIMITS(eu)
        __FPM_LIMITS(aeu)
        __FPM_LIMITS(emu)
        __FPM_LIMITS(aemu)
        __FPM_LIMITS(em)
        __FPM_LIMITS(aem)
        __FPM_LIMITS(eml)
        __FPM_LIMITS(aeml)
        __FPM_LIMITS(el)
        __FPM_LIMITS(ael)
        #endif


#ifdef FPM_CUSTOM_MULT
    namespace custom
    {
        #include "fpm_custom.inc"
    } // namespace custom
    using namespace custom;
#else
    using namespace native;
#endif

    }
}
