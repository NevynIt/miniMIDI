#pragma once

#include "wave_root.h"

namespace dsp::exec
{
    using namespace ::dsp;
    static constexpr bool debug = false;

    //bit count
    //5 for the opcode //32 opcodes
    //5 for the target //32 targets
    //5 for the source //32 sources
    //1 bit for a flag

    //source/target id, for all except sel, lookup and store, mean:
    // 0 output or input register
    // 1-2 immediate registers x, y
    // 3-18 scratchpad registers S[0-15]
    // 19-31 indirect registers I[0-12]

    typedef union 
    { // todo redefine as packed union with bitfields
        struct 
        { //mov
            uint16_t cmd : 5, tgt : 5, src : 5, flag : 1;
        };
        uint16_t raw;
    } newOpCode;

    typedef union
    {
        struct
        {
            uint16_t id : 8, index : 8;
        };
        uint16_t raw;
    } op_sel_param;

    //source can be an immediate register, a small constant, or a literal value stored after the opcode
    enum newcmd
    {
        // basic commands
        load, //loads a literal into an immediate register
                //tgt selects the register
                //src is the number of literals to load in sequence
                //flag is ignored
                //opcode + 1 is the first literal, and so on
                //src opcodes are consumed by the load command
        sel, //sets the pointer of one of the indirect registers
                //tgt selects the indirect register, (0 to 31)
                //src selects between wave parameter or global buffer (0 to 1)
                //flag tells if an optional index is included, which then consumes one more opcode
                //for wave, opcode + 1 is 8 bit for the wave id, and 8 bit for the parameter id
                //for global buffer, opcode + 1 is 8 bit for the buffer id, and 8 bit for the index offset
        mov, //moves the content of the source register to the target register
                //tgt selects the target register
                //src selects the source register
                //if flag is set, the value is the 5 bits of src, interpreted as signed 5 bit value (2's complement)
        ret, //returns the content of the output register
                //all other fields are ignored
        inc, //increments the immediate register
                //tgt selects the register
                //src is ignored
                //flag is ignored
        dec, //decrements the immediate register
                //tgt selects the register
                //src is ignored
                //flag is ignored
        add, //adds the content of the source register to the target register
                //tgt selects the target register
                //src selects the source register
                //if flag is set, the value is the 5 bits of src, interpreted as signed 5 bit value (2's complement)
        sub, //subtracts the content of the source register from the target register
                //tgt selects the target register
                //src selects the source register
                //if flag is set, the value is the 5 bits of src, interpreted as signed 5 bit value (2's complement)
        mul, //multiplies the content of the source register with the target register
                //tgt selects the target register
                //src selects the source register
                //if flag is set, the value is the 5 bits of src, interpreted as signed 5 bit value (2's complement)
        lhs, //shifts the content of the target register left by the source register
                //tgt selects the target register
                //src selects the source register
                //if flag is set, the value is the 5 bits of src, interpreted as signed 5 bit value (2's complement)
        rhs, //shifts the content of the target register right by the source register
                //tgt selects the target register
                //src selects the source register
                //if flag is set, the value is the 5 bits of src, interpreted as signed 5 bit value (2's complement)
        jmp, //checks the content of the source register (could be immediate) and jumps to a label id if it is zero
                //tgt is the label id
                //src selects the source register
                //if flag is set, the value is the 5 bits of src, interpreted as signed 5 bit value (2's complement)
        call, //same as jmp, but as a function call, return will come back here instead of exiting the function
                //tgt is the label id
                //src is ignored
                //flag is ignored

        // special DSP commands
        biquad, //applies a biquad filter, the output register stores the result
                //tgt is the pointer to the state variables (4 registers), must be an indirect register
                //src is the pointer to the coefficients (5 registers), must be an indirect register
                //if flag is 0, the input parameter is used as input, if flag is 1, the output register is used as input
                //coefficients are saved as b0, b1, b2, a1, a2
                //state variables are saved as x1, x2, y1, y2
        scale,  //scales the content of the target register with the source register
                //tgt selects the target register
                //src selects the source register, if flag is set, the value is the 5 bits of src, interpreted as 32 levels of scaling
        store,  //store the value of the output register in a buffer
                //tgt selects the indirect register that points to the buffer (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
        lkp_arr,//loads a value from an array in the output register
                //tgt selects the indirect register that points to the array (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
        lkp_64, //loads a value from a function lookup table of size 64 in the output register
                //tgt selects the indirect register that points to the lookup table (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
                //the first 6 bits of the index are used to select the value, the next 6 bits are ignored
        lkp_256,//loads a value from a function lookup table of size 256 in the output register
                //tgt selects the indirect register that points to the lookup table (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
                //the first 8 bits of the index are used to select the value, the next 8 bits are ignored
        lkp_1024,//loads a value from a function lookup table of size 1024 in the output register
                //tgt selects the indirect register that points to the lookup table (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
                //the first 10 bits of the index are used to select the value, the next 6 bits are ignored
        lkp_4096,//loads a value from a function lookup table of size 4096 in the output register
                //tgt selects the indirect register that points to the lookup table (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
                //the first 12 bits of the index are used to select the value, the next 4 bits are ignored
        lkp_64i,//loads a value from a function lookup table of size 64x2 in the output register
                //tgt selects the indirect register that points to the lookup table (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
                //the first 6 bits of the index are used to select the value, the next 6 bits are used to interpolate
                //the lookup table is stored as 0: value 0 -- 1: delta to value 1 (signed 16 bit) -- 2: value 1 -- 3: delta to value 2 (signed 16 bit)
        lkp_256i,//loads a value from a function lookup table of size 256x2 in the output register
                //tgt selects the indirect register that points to the lookup table (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
                //the first 8 bits of the index are used to select the value, the next 8 bits are used to interpolate
                //the lookup table is stored as 0: value 0 -- 1: delta to value 1 (signed 16 bit) -- 2: value 1 -- 3: delta to value 2 (signed 16 bit)
        lkp_1024i,//loads a value from a function lookup table of size 1024x2 in the output register
                //tgt selects the indirect register that points to the lookup table (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
                //the first 10 bits of the index are used to select the value, the next 6 bits are used to interpolate
                //the lookup table is stored as 0: value 0 -- 1: delta to value 1 (signed 16 bit) -- 2: value 1 -- 3: delta to value 2 (signed 16 bit)
        lkp_4096i,//loads a value from a function lookup table of size 4096x2 in the output register
                //tgt selects the indirect register that points to the lookup table (0 to 31)
                //src selects the index in the buffer, if flag is set, the index is the 5 bits of src, interpreted as unsigned 5 bit value
                //the first 12 bits of the index are used to select the value, the next 4 bits are used to interpolate
                //the lookup table is stored as 0: value 0 -- 1: delta to value 1 (signed 16 bit) -- 2: value 1 -- 3: delta to value 2 (signed 16 bit)
        advance,//advances the wave operator
                //tgt is ignored if flag is 0
                //src is the wave id (0 to 31)
                //if flag is 1 then the wave is advanced a number of times equal to the 5 bits of tgt, interpreted as unsigned 5 bit value
        sample, //samples the output of one wave operator, and stores it in the output register
                //tgt is the where to store the result
                //src is the wave id (0 to 31)
                //if flag is 1 then the wave is also advanced once

        // user defined commands
        usr0, //custom command 0, get the call parameter from src (flag means literal), store the result in tgt, the command must advance the code pointer
        usr1, //custom command 1, get the call parameter from src (flag means literal), store the result in tgt, the command must advance the code pointer
        usr2, //custom command 2, get the call parameter from src (flag means literal), store the result in tgt, the command must advance the code pointer
        usr3, //custom command 3, get the call parameter from src (flag means literal), store the result in tgt, the command must advance the code pointer
        opcode_count
    };

    class context;

    typedef SampleType (*usr_fnc_t)(context &ctx, const SampleType in_reg);

    SampleType noop(context &ctx, const SampleType in_reg);

    class context
    {
    public:
        SampleType *S = nullptr; //scratchpad registers
        SampleType **I = nullptr; //indirect registers
        const newOpCode **jmp_tgt = nullptr; //jump targets
        const newOpCode *pc = nullptr;
        const newOpCode *call_stack[16] = {nullptr};
        uint8_t call_stack_ptr = 0;
        usr_fnc_t usr_fnc[4] = {nullptr};
        wave *ops = nullptr;

        context(SampleType *S, SampleType **I, wave *ops, const newOpCode **jmp_tgt, const newOpCode *pc)
            : S(S), I(I), ops(ops), jmp_tgt(jmp_tgt), pc(pc)
        {
            usr_fnc[0] = noop;
            usr_fnc[1] = noop;
            usr_fnc[2] = noop;
            usr_fnc[3] = noop;
        }

        inline void jmp_abs(const newOpCode *tgt)
        {
            pc = tgt;
        }

        inline void jmp(const uint8_t label_id)
        {
            pc = jmp_tgt[label_id];
        }

        inline void call(const uint8_t label_id)
        {
            call_stack[call_stack_ptr++] = pc;
            pc = jmp_tgt[label_id];
        }

        inline void call_abs(const newOpCode *tgt)
        {
            call_stack[call_stack_ptr++] = pc;
            pc = tgt;
        }

        inline void ret()
        {
            if (debug && call_stack_ptr == 0)
            {
                printf("Error: return without call in dsp asm\n");
                __breakpoint();
            }
            pc = call_stack[--call_stack_ptr];
        }
        
        SampleType operator ()(const SampleType in_reg = 0, const newOpCode *code = nullptr);
    };
}