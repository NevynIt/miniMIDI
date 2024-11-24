#pragma once

#include "wave_logic_wave.h"

namespace dsp
{
    template <typename Logic>
    class script_wave : public logic_wave<Logic>
    {
    public:
        static constexpr auto signature = Logic::signature + uti::STR2A(" scriptable");
        script_wave() = default;

        // wave overload
        virtual const char *getSignature() const override { return signature.data(); }

        virtual SampleType operator()() override { return getSample(); }
        virtual void operator++() override { advance(); }

        inline SampleType getSample()
        {
            if (code[function::fnc_preSampling])
                run(code[function::fnc_preSampling], code_size[function::fnc_preSampling]);
            this->logic.preSampling();
            logic_wave<Logic>::gatherOutput();
            ret = this->logic.postSampling();
            if (code[function::fnc_postSampling])
                run(code[function::fnc_postSampling], code_size[function::fnc_postSampling]);
            return ret;
        }

        inline void advance()
        {
            if (code[function::fnc_preAdvance])
                run(code[function::fnc_preAdvance], code_size[function::fnc_preAdvance]);
            this->logic.preAdvance();
            logic_wave<Logic>::advanceOps();
            this->logic.postAdvance();
            if (code[function::fnc_postAdvance])
                run(code[function::fnc_postAdvance], code_size[function::fnc_postAdvance]);
        }

        virtual inline void attack() override
        {
            logic_wave<Logic>::attack();
            if (code[function::fnc_attack])
                run(code[function::fnc_attack], code_size[function::fnc_attack]);
        } 

        virtual inline void release() override
        {
            logic_wave<Logic>::release();
            if (code[function::fnc_release])
                run(code[function::fnc_release], code_size[function::fnc_release]);
        }

        virtual inline void stop() override
        {
            logic_wave<Logic>::stop();
            if (code[function::fnc_stop])
                run(code[function::fnc_stop], code_size[function::fnc_stop]);
        }

        enum function
        {
            fnc_preSampling,
            fnc_postSampling,
            fnc_preAdvance,
            fnc_postAdvance,
            fnc_attack,
            fnc_release,
            fnc_stop,
            fnc_count,
        };

        void setCode(const function fnc, const dsp::OpCode *prg, uint16_t size)
        {
            this->code[fnc] = prg;
            this->code_size[fnc] = size;
        }

        inline SampleType *getVar(uint16_t bank, uint16_t id, bool safe = true)
        {
            SampleType *ptr = nullptr;
            switch (bank)
            {
            case 0:
                if (id < sizeof(Logic::S) / sizeof(SampleType))
                    return &((SampleType *)&this->logic.S)[id];
                break;
            case 1:
                if (id < sizeof(Logic::I) / sizeof(SampleType *))
                    return ((SampleType **)&this->logic.I)[id];
                break;
            case 2:
                if (id < sizeof(Logic::D) / sizeof(DynamicReference))
                    return ((DynamicReference *)&this->logic.D)[id]();
                break;
            }
            if (ptr == nullptr && safe)
                return &sample_dump; //pointers will point here if undefined
            return ptr;
        }
        
        inline void op_Adv(const OpCode& instr)
        {
            if (instr.immediate)
            {
                AdvanceOp<Logic::op_count, Logic>::apply(this->logic, instr.mask);
            }
            else
                this->logic.advance_mask = instr.mask;
        }

        inline void op_Out(const OpCode& instr)
        {
            if (instr.immediate)
            {
                GatherOutput<Logic::op_count, Logic>::apply(this->logic, instr.mask);
            }
            else
                this->logic.gather_mask = instr.mask;
        }

        inline void op_Ldl(const OpCode& instr)
        {
            if (instr.tgt_ld < sizeof(Logic::S) / sizeof(SampleType))
            {
                SampleType *ptr = &((SampleType *)&this->logic.S)[instr.tgt_ld];
                *ptr = *ptr & 0xFF00 | instr.literal;
            }
        }

        inline void op_Ldh(const OpCode& instr)
        {
            if (instr.tgt_ld < sizeof(Logic::S) / sizeof(SampleType))
            {
                SampleType *ptr = &((SampleType *)&this->logic.S)[instr.tgt_ld];
                *ptr = *ptr & 0x00FF | instr.literal << 8;
            }
        }

        inline void op_Sel(const OpCode& instr)
        { 
            if (instr.tgt_id >= sizeof(Logic::I) / sizeof(SampleType *))
                return;

            op::IndirectIdentifier identifier = {.raw = (uint16_t)*getVar(instr.src_bank, instr.src_id) };

            switch (instr.sel_type)
            {
            case 0: //wave parameter
                if (identifier.param_id == op::output_param_id)
                    ((SampleType **)&this->logic.I)[instr.tgt_id] = &this->logic.output[identifier.wave_id];
                else 
                    ((SampleType **)&this->logic.I)[instr.tgt_id] = this->logic.ops.get_wave(identifier.wave_id)->getParam(identifier.param_id);
                break;
            case 1: // fixed address
                ((SampleType **)&this->logic.I)[instr.tgt_id] = globalLookup(identifier.buffer_id, identifier.fixed_address << 8);
                break;
            case 2: // dynamic lookup
            {
                if (instr.tgt_id >= sizeof(Logic::D) / sizeof(DynamicReference))
                    return;
                DynamicReference &d = ((DynamicReference *)&this->logic.D)[instr.tgt_id];
                d.index = ((SampleType **)&this->logic.I)[instr.tgt_id];
                d.buffer_id = identifier.buffer_id;
                d.shift_dir = identifier.shift_dir;
                d.shift_bits = identifier.shift_bits;
            }
                break;
            }
        }

        inline void op_Mov(const OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) = *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Add(const OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) += *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Sub(const OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) -= *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Mul(const OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) *= *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Sca(const OpCode& instr)
        {
            int32_t result = (int32_t)(*getVar(instr.tgt_bank, instr.tgt_id)) * *getVar(instr.src_bank, instr.src_id);
            *getVar(instr.tgt_bank, instr.tgt_id) = result >> 16;
        }

        inline void op_Lhs(const OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) <<= *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Rhs(const OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) >>= *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Ret(const OpCode& instr)
        {
            ret = *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Usr1(const OpCode& instr)
        {
            this->logic.op_Usr1(instr);
        }

        inline void op_Usr2(const OpCode& instr)
        {
            this->logic.op_Usr2(instr);
        }

        inline void op_Usr3(const OpCode& instr)
        {
            this->logic.op_Usr3(instr);
        }

        inline void exec(const OpCode &instr)
        {
            using OpFunc = void (script_wave::*)(const OpCode& instr);
            static constexpr OpFunc opFuncs[] = {
                &script_wave::op_Adv, &script_wave::op_Out, &script_wave::op_Ldl, &script_wave::op_Ldh,
                &script_wave::op_Sel, &script_wave::op_Mov, &script_wave::op_Add, &script_wave::op_Sub,
                &script_wave::op_Mul, &script_wave::op_Sca, &script_wave::op_Lhs, &script_wave::op_Rhs,
                &script_wave::op_Ret //still space for 3 more instructions left
            };
            (this->*opFuncs[instr.cmd])(instr);
        }

        inline void run(const OpCode *code, uint16_t size)
        {
            for (pc = 0; pc < size; pc++)
                exec(code[pc]);
        }

        // deep inspection
        void inspect(int indent = 0) override
        {
            logic_wave<Logic>::inspect(indent); // for now, just call the base class
            for (int i = 0; i < function::fnc_count; i++)
            {
                if (code[i])
                {
                    for (int j = 0; j < indent; ++j) printf(indent_str);
                    printf("  Function %d:\n", i);
                    for (int j = 0; j < indent; ++j) printf(indent_str);
                    printf("    Code (%d):\n", code_size[i]);
                    for (int j = 0; j < code_size[i]; j++)
                    {
                        for (int k = 0; k < indent; ++k) printf(indent_str);
                        printf("      %d: %s\n", j, disassemble(code[i][j]).c_str());
                    }
                }
            }
        }

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
                    //src selects between wave parameter, wave buffer or global buffer (0 to 2)
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
                    //if flag is 0, the input register is used as input, if flag is 1, the output register is used as input
            sca, //scales the content of the target register with the source register
                    //tgt selects the target register
                    //src selects the source register
                    //if flag is set, the value is the 5 bits of src, interpreted as 32 levels of scaling
            adv, //advances one or more wave operators, and/or sets the mask
            out, //gathers the output of one or more wave operators, and/or sets the mask
            lookup, //loads a value from a buffer in the output register
                    //tgt selects the indirect register that points to the buffer (0 to 31)
                    //src selects the index in the buffer
                    //flag is ignored
            store, //store the value of the output register in a buffer
                    //tgt selects the indirect register that points to the buffer (0 to 31)
                    //src selects the index in the buffer
                    //flag is ignored
            // user defined commands
            usr1, //logic specific command 1 -- *tgt = usr1(*src)
            usr2, //logic specific command 2 -- *tgt = usr2(*src)
            usr3, //logic specific command 3 -- *tgt = usr3(*src)
            usr4, //logic specific command 4 -- *tgt = usr4(*src)
        };

        SampleType new_exec_code(const newOpCode *code, const SampleType in_reg)
        {   /* new opcode idea and opcode set
            * we have local variables as immediate registers, some of type SampleType, some of type SampleType *, and some more
            * the opcode, which is 16 bits may be followed by more 16 bits of immediate data, depending on the opcode 
            * the opcodes we need are:
            * 
            */
            SampleType out = 0;
            SampleType x = 0, y = 0;
            SampleType *S[16] = ;
            SampleType *S = ((SampleType *)&this->logic.S); //move S and I to a a member variable of a class
            SampleType **I = ((SampleType **)&this->logic.I);

            uint8_t cmd;
            while ((cmd = code->cmd) != newcmd::ret)
            {
                switch (cmd)
                {
                case newcmd::load:
                {
                    SampleType *tgt;
                    switch (code->tgt)
                    {
                    case 0:
                        tgt = &out;
                        break;
                    case 1:
                        tgt = &x;
                        break;  
                    case 2:
                        tgt = &y;
                        break;
                    default:
                        if (code->tgt < 19)
                            tgt = &S[code->tgt - 3];
                        else
                            tgt = I[code->tgt - 19];
                        break;
                    }

                    for (int i = 0; i < code->src; i++)
                    {
                        *(tgt++) = (SampleType)(*(++code));
                    }
                }
                    break;
                case newcmd::sel:
                {
                    switch (code->src)
                    {
                    case 0: //wave parameter
                    {
                        const op_sel_param param = {.raw = (uint16_t)(*(++code))};
                        if (param.id == op::output_param_id)
                            I[code->tgt] = &this->logic.output[param.index];
                        else
                            I[code->tgt] = this->logic.ops.get_wave(param.id)->getParam(param.index);
                        if (code->flag)
                            code++;
                    }
                        break;
                    case 1: //wave buffer
                    {
                        const op_sel_param param = {.raw = (uint16_t)(*(++code))};
                        SampleType index = 0;
                        if (code->flag)
                            index = (SampleType)(*(++code)); //assumption is that sampletype is 16 bits
                        I[code->tgt] = this->logic.Lookup(param.id, param.index + index);
                    }
                        break;
                    case 2: //global buffer
                    {
                        SampleType index = 0;
                        if (code->flag)
                            index = (SampleType)(*(++code)); //assumption is that sampletype is 16 bits
                        I[code->tgt] = globalLookup(code->src, index);
                    }
                        break;
                    default:
                        if (code->flag)
                            code++;
                        break;
                    }
                }
                    break;
                default:
                    break;
                }
            }
            return out_reg;
        }

    public:
        const OpCode *code[function::fnc_count] = {nullptr};
        uint16_t code_size[function::fnc_count] = {0};
        SampleType ret = 0;
        uint16_t pc = 0;
    };
}