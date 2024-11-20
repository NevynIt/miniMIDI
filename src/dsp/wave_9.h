#pragma once

#include "wave_8.h"

namespace dsp
{
    template <typename impl>
    class scriptable_base : public wave_base<impl>
    {
    public:
        scriptable_base() = default;

        // wave overload
        virtual const char *getSignature() const override { return impl.signature.data(); }

        virtual SampleType operator()() impl.override { return getSample(); }
        virtual void operator++() override { advance(); }

        inline SampleType getSample()
        {
            if (code[function::fnc_preSampling])
                run(code[function::fnc_preSampling], code_size[function::fnc_preSampling]);
            impl.preSampling();
            impl.gatherOutput();
            ret = impl.postSampling();
            if (code[function::fnc_postSampling])
                run(code[function::fnc_postSampling], code_size[function::fnc_postSampling]);
            return ret;
        }

        inline void advance()
        {
            if (code[function::fnc_preAdvance])
                run(code[function::fnc_preAdvance], code_size[function::fnc_preAdvance]);
            impl.preAdvance();
            advanceOps();
            impl.postAdvance();
            if (code[function::fnc_postAdvance])
                run(code[function::fnc_postAdvance], code_size[function::fnc_postAdvance]);
        }

        virtual inline void attack() override
        {
            wave_base<impl>::attack();
            if (code[function::fnc_attack])
                run(code[function::fnc_attack], code_size[function::fnc_attack]);
        } 

        virtual inline void release() override
        {
            wave_base<impl>::release();
            if (code[function::fnc_release])
                run(code[function::fnc_release], code_size[function::fnc_release]);
        }

        virtual inline void stop() override
        {
            wave_base<impl>::stop();
            if (code[function::fnc_stop])
                run(code[function::fnc_stop], code_size[function::fnc_stop]);
        }

        void setCode(const function fnc, const op::OpCode *prg, uint16_t size)
        {
            this->code[fnc] = prg;
            this->program_size = size;
        }

        inline SampleType *getVar(uint16_t bank, uint16_t id, bool safe = true)
        {
            SampleType *ptr = nullptr;
            switch (bank)
            {
            case 0:
                if (id < sizeof(impl::S) / sizeof(SampleType))
                    return &((SampleType *)&impl::S)[id];
                break;
            case 1:
                if (id < sizeof(impl::I) / sizeof(SampleType *))
                    return ((SampleType **)&impl::I)[id];
                break;
            case 2:
                if (id < sizeof(impl::D) / sizeof(DynamicReference))
                    return ((DynamicReference *)&impl::D)[id]();
                break;
            }
            if (ptr == nullptr && safe)
                return &sample_dump; //pointers will point here if undefined
            return ptr;
        }
        
        inline void op_Adv(const op::OpCode& instr)
        {
            if (instr.immediate)
            {
                ADVANCE_OP(impl.op_count, instr.mask);
            }
            else
                advance_mask = instr.mask;
        }

        inline void op_Out(const op::OpCode& instr)
        {
            if (instr.immediate)
            {
                GATHER_OUTPUT(impl.op_count, instr.mask);
            }
            else
                output_mask = instr.mask;
        }

        inline void op_Ldl(const op::OpCode& instr)
        {
            if (instr.tgt_ld < sizeof(impl::S) / sizeof(SampleType))
            {
                SampleType *ptr = ((SampleType *)&impl::S)[instr.tgt_ld];
                *ptr = *ptr & 0xFF00 | instr.literal;
            }
        }

        inline void op_Ldh(const op::OpCode& instr)
        {
            if (instr.tgt_ld < sizeof(impl::S) / sizeof(SampleType))
            {
                SampleType *ptr = ((SampleType *)&impl::S)[instr.tgt_ld];
                *ptr = *ptr & 0x00FF | instr.literal << 8;
            }
        }

        inline void op_Sel(const op::OpCode& instr)
        { 
            if (instr.tgt_id >= sizeof(impl::I) / sizeof(SampleType *))
                return;

            IndirectIdentifier identifier = {.raw = (uint16_t)*getVar(instr.src_bank, instr.src_id) };

            switch (instr.sel_type)
            {
            case 0: //wave parameter
                if (identifier.param_id == output_param_id)
                    ((SampleType **)&impl::I)[instr.tgt_id] = &output[identifier.wave_id];
                else 
                    ((SampleType **)&impl::I)[instr.tgt_id] = impl.ops.get_wave(identifier.wave_id).getParam(identifier.param_id);
                break;
            case 1: // fixed address
                ((SampleType **)&impl::I)[instr.tgt_id] = globalLookup(identifier.buffer_id, identifier.fixed_address << 8);
                break;
            case 2: // dynamic lookup
            {
                if (instr.tgt_id >= sizeof(impl::D) / sizeof(DynamicReference))
                    return;
                DynamicReference &d = ((DynamicReference *)&impl::D)[id];
                d.index = ((SampleType **)&impl::I)[instr.tgt_id];
                d.buffer_id = identifier.buffer_id;
                d.shift_dir = identifier.shift_dir;
                d.shift_bits = identifier.shift_bits;
            }
                break;
            }
        }

        inline void op_Mov(const op::OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) = *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Add(const op::OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) += *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Sub(const op::OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) -= *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Mul(const op::OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) *= *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Sca(const op::OpCode& instr)
        {
            int32_t result = (int32_t)(*getVar(instr.tgt_bank, instr.tgt_id)) * *getVar(instr.src_bank, instr.src_id);
            *getVar(instr.tgt_bank, instr.tgt_id) = result >> 16;
        }

        inline void op_Lhs(const op::OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) <<= *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Rhs(const op::OpCode& instr)
        {
            *getVar(instr.tgt_bank, instr.tgt_id) >>= *getVar(instr.src_bank, instr.src_id);
        }

        inline void op_Ret(const op::OpCode& instr)
        {
            ret = *getVar(instr.src_bank, instr.src_id);
        }

        inline void exec(const op::OpCode &instr)
        {
            using OpFunc = void (paramWave::*)(const op::OpCode& instr);
            static constexpr OpFunc opFuncs[] = {
                &paramWave::op_Adv, &paramWave::op_Out, &paramWave::op_Ldl, &paramWave::op_Ldh,
                &paramWave::op_Sel, &paramWave::op_Mov, &paramWave::op_Add, &paramWave::op_Sub,
                &paramWave::op_Mul, &paramWave::op_Sca, &paramWave::op_Lhs, &paramWave::op_Rhs,
                &paramWave::op_Ret //still space for 3 more instructions left
            };
            (this->*opFuncs[instr.cmd])(instr);
        }

        inline void run(const op::OpCode *code, uint16_t size)
        {
            current_function = fnc_none;
            for (pc = 0; pc < size; pc++)
                exec(code[pc]);
        }

    public:
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

        const op::OpCode *code[function::fnc_count] = {nullptr};
        uint16_t code_size[function::fnc_count] = {0};
        SampleType ret = 0;
        uint16_t pc = 0;
    };
}