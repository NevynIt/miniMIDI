#include "exec_asm.h"

namespace dsp::exec
{
    using namespace ::dsp;

    SampleType noop(context &ctx, SampleType in_reg)
    {
        return in_reg;
    }

    SampleType context::operator ()(const SampleType in_reg = 0, const newOpCode *code = nullptr)
    {   /* new opcode idea and opcode set
        * we have local variables as immediate registers, some of type SampleType, some of type SampleType *, and some more
        * the opcode, which is 16 bits may be followed by more 16 bits of immediate data, depending on the opcode 
        * the opcodes we need are:
        * 
        */
        if (code == nullptr)
            code = this->pc;
        
        SampleType out = in_reg;
        SampleType x = 0, y = 0;

        uint8_t cmd;
        while (1) //crazy infinite loop, the code should be statically checked to avoid infinite loops
        {
            switch (cmd)
            {
            case newcmd::load:
            {
                switch (code->tgt)
                {
                case 0:
                    out = (SampleType)((++code)->raw);
                    code += code->src;
                    break;
                case 1:
                    x = (SampleType)((++code)->raw);
                    code += code->src;
                    break;  
                case 2:
                    y = (SampleType)((++code)->raw);
                    code += code->src;
                    break;
                default:
                {
                    SampleType *tgt;
                    if (code->tgt < 19)
                        tgt = &S[code->tgt - 3];
                    else
                        tgt = I[code->tgt - 19];
                    for (int i = 0; i < code->src; i++)
                    {
                        *(tgt++) = (SampleType)((++code)->raw);
                    }
                    code++;
                }
                    break;
                }
            }
                break;

            case newcmd::sel:
            {
                const op_sel_param param = {.raw = (code+1)->raw};
                switch (code->src & 1)
                {
                case 0: //wave parameter
                    I[code->tgt] = ops[param.id].getParam(param.index);
                    break;
                case 1: //global buffer
                    if (code->flag)
                        I[code->tgt] = globalLookup(param.id, param.index + (SampleType)(code+2)->raw);
                    else
                        I[code->tgt] = globalLookup(param.id, param.index);
                    break;
                }
                if (code->flag)
                    code+=3;
                else
                    code+=2;
            }
                break;

            case newcmd::mov:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    if (v & 0x10)
                        v |= 0xFFE0;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out = v;
                    break;
                case 1:
                    x = v;
                    break;
                case 2:
                    y = v;
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] = v;
                    else
                        *I[code->tgt - 19] = v;
                }
                    break;
                }
                code++;
            }
                break;

            case newcmd::ret:
                if (call_stack_ptr == 0) //return from main
                    return out;
                code = call_stack[--call_stack_ptr]; //return from function call
                break;
            
            case newcmd::inc:
            {
                switch (code->tgt)
                {
                case 0:
                    out++;
                    break;
                case 1:
                    x++;
                    break;
                case 2:
                    y++;
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3]++;
                    else
                        *I[code->tgt - 19]++;
                }
                    break;
                }
                code++;
            }
                break;

            case newcmd::dec:
            {
                switch (code->tgt)
                {
                case 0:
                    out--;
                    break;
                case 1:
                    x--;
                    break;
                case 2:
                    y--;
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3]--;
                    else
                        *I[code->tgt - 19]--;
                }
                    break;
                }
                code++;
            }
                break;

            case newcmd::add:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    if (v & 0x10)
                        v |= 0xFFE0;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out += v;
                    break;
                case 1:
                    x += v;
                    break;
                case 2:
                    y += v;
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] += v;
                    else
                        *I[code->tgt - 19] += v;
                }
                    break;
                }
                code++;
            }
                break;

            case newcmd::sub:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    if (v & 0x10)
                        v |= 0xFFE0;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out -= v;
                    break;
                case 1:
                    x -= v;
                    break;
                case 2:
                    y -= v;
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] -= v;
                    else
                        *I[code->tgt - 19] -= v;
                }
                    break;
                }
                code++;
            }
                break;
            
            case newcmd::mul:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    if (v & 0x10)
                        v |= 0xFFE0;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out *= v;
                    break;
                case 1:
                    x *= v;
                    break;
                case 2:
                    y *= v;
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] *= v;
                    else
                        *I[code->tgt - 19] *= v;
                }
                    break;
                }
                code++;
            }
                break;
            
            case newcmd::lhs:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    if (v & 0x10)
                        v |= 0xFFE0;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out <<= v;
                    break;
                case 1:
                    x <<= v;
                    break;
                case 2:
                    y <<= v;
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] <<= v;
                    else
                        *I[code->tgt - 19] <<= v;
                }
                    break;
                }
                code++;
            }
                break;

            case newcmd::rhs:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    if (v & 0x10)
                        v |= 0xFFE0;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out >>= v;
                    break;
                case 1:
                    x >>= v;
                    break;
                case 2:
                    y >>= v;
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] >>= v;
                    else
                        *I[code->tgt - 19] >>= v;
                }
                    break;
                }
                code++;
            }
                break;

            case newcmd::jmp:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    if (v & 0x10)
                        v |= 0xFFE0;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                if (v == 0)
                    jmp((uint8_t)code->tgt);
                else
                    code++;
            }
                break;

            case newcmd::call:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    if (v & 0x10)
                        v |= 0xFFE0;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                if (v == 0)
                    call((uint8_t)code->tgt);
                else
                    code++;
            }
                break;

            case newcmd::biquad:
            {
                using tmpDescr = ::fpm::mul_result_struct<StateDescr, CoeffDescr>;
                using ee = tmpDescr::t;
                const StateType in = ::fpm::conv<StateDescr, SampleDescr>(code->flag ? out : in_reg);
                StateType *state = (StateType *)I[code->tgt];
                const CoeffType *coeff = (CoeffType *)I[code->src];
                //coefficients are saved as b0, b1, b2, a1, a2
                //state variables are saved as x1, x2, y1, y2
                
                ee tmp1 = (ee)coeff[0]*in 
                    + (ee)coeff[1]*state[0] 
                    + (ee)coeff[2]*state[1] 
                    - (ee)coeff[3]*state[2] 
                    - (ee)coeff[4]*state[3];
            
                StateType tmp2 = ::fpm::conv<StateDescr, tmpDescr>(tmp1);
                state[1] = state[0];
                state[0] = in;
                state[3] = state[2];
                state[2] = tmp2;

                out = ::fpm::conv<SampleDescr, StateDescr>(tmp2);
            }
                break;

            case newcmd::scale:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    v <<= 11;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out = fpm::mul<SampleDescr, SampleDescr, LevelDescr>(out, v);
                    break;
                case 1:
                    x = fpm::mul<SampleDescr, SampleDescr, LevelDescr>(x, v);
                    break;
                case 2:
                    y = fpm::mul<SampleDescr, SampleDescr, LevelDescr>(y, v);
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] = fpm::mul<SampleDescr, SampleDescr, LevelDescr>(S[code->tgt - 3], v);
                    else
                        *I[code->tgt - 19] = fpm::mul<SampleDescr, SampleDescr, LevelDescr>(*I[code->tgt - 19], v);
                }
                    break;
                }
                code++;
            }
                break;
            
            case newcmd::store:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                SampleType *arr = I[code->tgt];
                arr[v] = out; //the index could be negative, and it is on purpose, and it's not checked either way, happy memory corruption day
                code++;
            }
                break;

            case newcmd::lkp_arr:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                SampleType *arr = I[code->tgt];
                out = arr[v]; //the index could be negative, and it is on purpose, and it's not checked either way, happy memory corruption day
                code++;
            }
                break;

            case newcmd::lkp_64:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    v <<= 1;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out >> 10;
                        break;
                    case 1:
                        v = x >> 10;
                        break;
                    case 2:
                        v = y >> 10;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3] >> 10;
                        else
                            v = *I[code->src - 19] >> 10;
                    }
                        break;
                    }
                }

                SampleType *arr = I[code->tgt];
                out = arr[(uint16_t)v & 0x3F];
                code++;
            }
                break;

            case newcmd::lkp_256:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    v <<= 3;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out >> 8;
                        break;
                    case 1:
                        v = x >> 8;
                        break;
                    case 2:
                        v = y >> 8;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3] >> 8;
                        else
                            v = *I[code->src - 19] >> 8;
                    }
                        break;
                    }
                }

                SampleType *arr = I[code->tgt];
                out = arr[(uint16_t)v & 0xFF];
                code++;
            }
                break;

            case newcmd::lkp_1024:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    v <<= 5;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out >> 6;
                        break;
                    case 1:
                        v = x >> 6;
                        break;
                    case 2:
                        v = y >> 6;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3] >> 6;
                        else
                            v = *I[code->src - 19] >> 6;
                    }
                        break;
                    }
                }

                SampleType *arr = I[code->tgt];
                out = arr[(uint16_t)v & 0x3FF];
                code++;
            }
                break;

            case newcmd::lkp_4096:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    v <<= 7;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out >> 4;
                        break;
                    case 1:
                        v = x >> 4;
                        break;
                    case 2:
                        v = y >> 4;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3] >> 4;
                        else
                            v = *I[code->src - 19] >> 4;
                    }
                        break;
                    }
                }

                SampleType *arr = I[code->tgt];
                out = arr[(uint16_t)v & 0xFFF];
                code++;
            }
                break;

            case newcmd::lkp_64i:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    v <<= 11;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                using fracDescr = ::fpm::descr<FPM_WIDTH, -10, false>;
                SampleType *arr = I[code->tgt];
                uint16_t idx = (uint16_t)(v >> 10) & 0x3F; //take the top 6 bits
                uint16_t frac = (uint16_t)v & 0x3FF; //take the bottom 10 bits
                out = arr[2*idx] + fpm::mul<SampleDescr, SampleDescr, fracDescr>(arr[2*idx + 1], frac);
                code++;
            }
                break;
            
            case newcmd::lkp_256i:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    v <<= 11;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                using fracDescr = ::fpm::descr<FPM_WIDTH, -8, false>;
                SampleType *arr = I[code->tgt];
                uint16_t idx = (uint16_t)(v >> 8) & 0xFF; //take the top 8 bits
                uint16_t frac = (uint16_t)v & 0x3FF; //take the bottom 8 bits
                out = arr[2*idx] + fpm::mul<SampleDescr, SampleDescr, fracDescr>(arr[2*idx + 1], frac);
                code++;
            }
                break;
            
            case newcmd::lkp_1024i:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    v <<= 11;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                using fracDescr = ::fpm::descr<FPM_WIDTH, -6, false>;
                SampleType *arr = I[code->tgt];
                uint16_t idx = (uint16_t)(v >> 6) & 0x3FF; //take the top 10 bits
                uint16_t frac = (uint16_t)v & 0x3FF; //take the bottom 6 bits
                out = arr[2*idx] + fpm::mul<SampleDescr, SampleDescr, fracDescr>(arr[2*idx + 1], frac);
                code++;
            }
                break;

            case newcmd::lkp_4096i:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                    v <<= 11;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                using fracDescr = ::fpm::descr<FPM_WIDTH, -4, false>;
                SampleType *arr = I[code->tgt];
                uint16_t idx = (uint16_t)(v >> 4) & 0xFFF; //take the top 12 bits
                uint16_t frac = (uint16_t)v & 0x3FF; //take the bottom 4 bits
                out = arr[2*idx] + fpm::mul<SampleDescr, SampleDescr, fracDescr>(arr[2*idx + 1], frac);
                code++;
            }
                break;

            case newcmd::advance:
            {
                wave *w = &ops[(uint8_t)code->src];
                if (code->flag)
                {
                    for (int i = 0; i < (uint8_t)code->tgt; i++)
                        *w++;
                }
                else
                    *w++;
                code++;
            }
                break;
            
            case newcmd::sample:
            {
                wave *w = &ops[(uint8_t)code->src];

                switch (code->tgt)
                {
                case 0:
                    out = (*w)();
                    break;
                case 1:
                    x = (*w)();
                    break;
                case 2:
                    y = (*w)();
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] = (*w)();
                    else
                        *I[code->tgt - 19] = (*w)();
                }
                    break;
                }
                if (code->flag)
                    *w++;
                code++;
            }
                break;

            case newcmd::usr0:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out = usr_fnc[0](*this, v);
                    break;
                case 1:
                    x = usr_fnc[0](*this, v);
                    break;
                case 2:
                    y = usr_fnc[0](*this, v);
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] = usr_fnc[0](*this, v);
                    else
                        *I[code->tgt - 19] = usr_fnc[0](*this, v);
                }
                    break;
                }
            }
                break;
            
            case newcmd::usr1:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out = usr_fnc[1](*this, v);
                    break;
                case 1:
                    x = usr_fnc[1](*this, v);
                    break;
                case 2:
                    y = usr_fnc[1](*this, v);
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] = usr_fnc[1](*this, v);
                    else
                        *I[code->tgt - 19] = usr_fnc[1](*this, v);
                }
                    break;
                }
            }
                break;

            case newcmd::usr2:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out = usr_fnc[2](*this, v);
                    break;
                case 1:
                    x = usr_fnc[2](*this, v);
                    break;
                case 2:
                    y = usr_fnc[2](*this, v);
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] = usr_fnc[2](*this, v);
                    else
                        *I[code->tgt - 19] = usr_fnc[2](*this, v);
                }
                    break;
                }
            }
                break;

            case newcmd::usr3:
            {
                SampleType v;
                if (code->flag)
                {
                    v = (SampleType)code->src;
                }
                else
                {
                    switch (code->src)
                    {
                    case 0:
                        v = out;
                        break;
                    case 1:
                        v = x;
                        break;
                    case 2:
                        v = y;
                        break;
                    default:
                    {
                        if (code->src < 19)
                            v = S[code->src - 3];
                        else
                            v = *I[code->src - 19];
                    }
                        break;
                    }
                }

                switch (code->tgt)
                {
                case 0:
                    out = usr_fnc[3](*this, v);
                    break;
                case 1:
                    x = usr_fnc[3](*this, v);
                    break;
                case 2:
                    y = usr_fnc[3](*this, v);
                    break;
                default:
                {
                    if (code->tgt < 19)
                        S[code->tgt - 3] = usr_fnc[3](*this, v);
                    else
                        *I[code->tgt - 19] = usr_fnc[3](*this, v);
                }
                    break;
                }
            }
                break;

            default:
                break;
            }
        }
        return out;
    }
}