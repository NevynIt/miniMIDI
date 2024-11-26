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
    // 0 output register
    // 1-2 immediate registers x, y
    // 3-18 scratchpad registers S[0-15]
    // 19-31 indirect registers I[0-12]

    typedef union 
    {
        struct 
        {
            uint16_t cmd : 5, tgt : 5, src : 5, flag : 1;
        };
        uint16_t raw;
    } newOpCode;

    typedef union
    {
        struct
        { //wave parameter
            uint16_t id : 8, index : 8;
        };
        struct
        { //global or relative index
            uint16_t /* id */: 8, : 2, src : 5, flag : 1;
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
                //src selects between wave parameter (0), global buffer 0-65535 with no index (1), global buffer 0-255 with index from a register (2), and relative to another indirect register, with index taken from a parameter (3)
                //flag tells if an optional index is included, which then consumes one more opcode
                //for wave, opcode + 1 is 8 bit for the wave id, and 8 bit for the parameter id (index)
                //for global buffer, src>>2 is lookup page, opcode + 1 is 16 bit for the buffer id (the extra index is added to 0)
                //for global buffer with index, src>>2 is lookup page, opcode + 1 is 8 bit for the buffer id, src and flag are used to fetch the index (the extra index is added to this)
                //for relative index, opcode + 1 is 8 bit for the indirect register (5 used), src&flag gets the base index offset from a register, see below, (the extra index is added to this)
        mov, //moves the content of the source register to the target register
                //tgt selects the target register
                //src selects the source register
                //if flag is set, the value is the 5 bits of src, interpreted as signed 5 bit value (2's complement)
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
        ext, //extended commands
                //tgt defines the extended command:
                    //0 is ret
                    //1 is attack //src is the wave id
                    //2 is release //src is the wave id
                    //3 is stop //src is the wave id
                    //4 is setup //src is the wave id

        // special DSP commands
        biquad, //applies a biquad filter, the output register stores the result
                //tgt is the pointer to the state variables (4 registers), must be an indirect register
                //src is the pointer to the coefficients (5 registers), must be an indirect register
                //flag is ignored, the output register is used as input
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
        adv,//advances the wave operator
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
        //possible extra operators: boolean operators, bitwise operators, comparison operators
        //I can remove the _64 and 4096 versions, getting 4 more opcodes
        //need to add attack, release and so on, I could add flags to ret, which has 11 bits unused, to do sub-opcodes
        opcode_count
    };

    class context;

    typedef SampleType (*usr_fnc_t)(context &ctx, const SampleType in_reg);

    SampleType noop(context &ctx, const SampleType in_reg);
    const newOpCode *get_default_codebase();
    const uint16_t *get_default_targets();
    const uint8_t get_default_targets_size();

    enum target_ids : uint8_t
    {
        getSample,
        advance,
        attack,
        release,
        stop,
        setup,
        user, //user defined targets from here
    };

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
        wave **ops = nullptr;

        //TODO get as parameter and add debug checks
        uint8_t S_size = 0; 
        uint8_t I_size = 0;
        uint8_t ops_size = 0;
        uint8_t jmp_tgt_size = 0;

        context()
        {
            usr_fnc[0] = noop;
            usr_fnc[1] = noop;
            usr_fnc[2] = noop;
            usr_fnc[3] = noop;
        }

        ~context()
        {
            delete[] ops;
            delete[] jmp_tgt;
        }

        void setState(SampleType *S, SampleType **I, uint8_t S_size, uint8_t I_size)
        {
            this->S = S;
            this->I = I;
            this->S_size = S_size;
            this->I_size = I_size;
        }

        void initState(SampleType *S, SampleType **I)
        {
            for (int i = 0; i < S_size; i++)
                this->S[i] = S[i];
            for (int i = 0; i < I_size; i++)
                this->I[i] = I[i];
        }

        void setOps(wave **ops, const uint8_t len)
        {
            this->ops = new wave *[len];
            for (int i = 0; i < len; i++)
                this->ops[i] = ops[i];
            ops_size = len;
        }

        void setCode(const newOpCode *codebase, const uint16_t *targets, const uint8_t len)
        {
            jmp_tgt = new const newOpCode *[len];
            for (int i = 0; i < len; i++)
                jmp_tgt[i] = codebase + targets[i];
            jmp_tgt_size = len;
            pc = codebase;
        }

        void setFnc(const uint8_t id, usr_fnc_t fnc)
        {
            usr_fnc[id] = fnc;
        }

        void inspect(int indent = 0)
        {
        }

        SampleType run(const newOpCode *code = nullptr);
    };
}