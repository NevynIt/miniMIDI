#pragma once

#include "wave_logic_wave.h"
#include "exec_asm.h"

namespace dsp
{
    template <typename Logic>
    class script_wave : public logic_wave<Logic>
    {
    public:
        static constexpr auto signature = Logic::signature + uti::STR2A(" scriptable");

        script_wave()
        {
            ctx.setState((SampleType *)&this->logic.S, (SampleType **)&this->logic.I, sizeof(Logic::S) / sizeof(SampleType), sizeof(Logic::I) / sizeof(SampleType *));
            //create an array with all the ops from logic, including this as op0, and set the ctx to point to it
            wave *ops = new wave[Logic::op_count + 1];
            ops[0] = *this;
            for (int i = 0; i < Logic::op_count; i++)
                ops[i + 1] = this->logic.ops.template get<i>();
            ctx.setOps(ops, Logic::op_count + 1);

            //start with some default code that just calls the logic implementation
            ctx.setCode(exec::get_default_codebase(), exec::get_default_targets(), exec::get_default_targets_size());

            ctx.setFnc(0, Logic::op_usr0);
            ctx.setFnc(1, Logic::op_usr1);
            ctx.setFnc(2, Logic::op_usr2);
            ctx.setFnc(3, Logic::op_usr3);
        }

        // wave overload
        virtual const char *getSignature() const override { return signature.data(); }

        virtual SampleType operator()() override { return getSample(); }
        virtual void operator++() override { advance(); }

        inline SampleType getSample()
        {
            return ctx.run(ctx.jmp_tgt[exec::target_ids::getSample]);
        }

        inline void advance()
        {
            ctx.run(ctx.jmp_tgt[exec::target_ids::advance]);
        }

        virtual inline void attack() override
        {
            ctx.run(ctx.jmp_tgt[exec::target_ids::attack]);
        }

        virtual inline void release() override
        {
            ctx.run(ctx.jmp_tgt[exec::target_ids::release]);
        }

        virtual inline void stop() override
        {
            ctx.run(ctx.jmp_tgt[exec::target_ids::stop]);
        }

        // deep inspection
        void inspect(int indent = 0) override
        {
            logic_wave<Logic>::inspect(indent);
            //then inspect the context
            ctx.inspect(indent);
        }

        exec::context ctx;
    };

    // template <typename Logic>
    // class script_wave : public logic_wave<Logic>
    // {
    // public:
    //     static constexpr auto signature = Logic::signature + uti::STR2A(" scriptable");
    //     script_wave() = default;

    //     // wave overload
    //     virtual const char *getSignature() const override { return signature.data(); }

    //     virtual SampleType operator()() override { return getSample(); }
    //     virtual void operator++() override { advance(); }

    //     inline SampleType getSample()
    //     {
    //         if (code[function::fnc_preSampling])
    //             run(code[function::fnc_preSampling], code_size[function::fnc_preSampling]);
    //         this->logic.preSampling();
    //         logic_wave<Logic>::gatherOutput();
    //         ret = this->logic.postSampling();
    //         if (code[function::fnc_postSampling])
    //             run(code[function::fnc_postSampling], code_size[function::fnc_postSampling]);
    //         return ret;
    //     }

    //     inline void advance()
    //     {
    //         if (code[function::fnc_preAdvance])
    //             run(code[function::fnc_preAdvance], code_size[function::fnc_preAdvance]);
    //         this->logic.preAdvance();
    //         logic_wave<Logic>::advanceOps();
    //         this->logic.postAdvance();
    //         if (code[function::fnc_postAdvance])
    //             run(code[function::fnc_postAdvance], code_size[function::fnc_postAdvance]);
    //     }

    //     virtual inline void attack() override
    //     {
    //         logic_wave<Logic>::attack();
    //         if (code[function::fnc_attack])
    //             run(code[function::fnc_attack], code_size[function::fnc_attack]);
    //     } 

    //     virtual inline void release() override
    //     {
    //         logic_wave<Logic>::release();
    //         if (code[function::fnc_release])
    //             run(code[function::fnc_release], code_size[function::fnc_release]);
    //     }

    //     virtual inline void stop() override
    //     {
    //         logic_wave<Logic>::stop();
    //         if (code[function::fnc_stop])
    //             run(code[function::fnc_stop], code_size[function::fnc_stop]);
    //     }

    //     enum function
    //     {
    //         fnc_preSampling,
    //         fnc_postSampling,
    //         fnc_preAdvance,
    //         fnc_postAdvance,
    //         fnc_attack,
    //         fnc_release,
    //         fnc_stop,
    //         fnc_count,
    //     };

    //     void setCode(const function fnc, const dsp::OpCode *prg, uint16_t size)
    //     {
    //         this->code[fnc] = prg;
    //         this->code_size[fnc] = size;
    //     }

    //     inline SampleType *getVar(uint16_t bank, uint16_t id, bool safe = true)
    //     {
    //         SampleType *ptr = nullptr;
    //         switch (bank)
    //         {
    //         case 0:
    //             if (id < sizeof(Logic::S) / sizeof(SampleType))
    //                 return &((SampleType *)&this->logic.S)[id];
    //             break;
    //         case 1:
    //             if (id < sizeof(Logic::I) / sizeof(SampleType *))
    //                 return ((SampleType **)&this->logic.I)[id];
    //             break;
    //         case 2:
    //             if (id < sizeof(Logic::D) / sizeof(DynamicReference))
    //                 return ((DynamicReference *)&this->logic.D)[id]();
    //             break;
    //         }
    //         if (ptr == nullptr && safe)
    //             return &sample_dump; //pointers will point here if undefined
    //         return ptr;
    //     }
        
    //     inline void op_Adv(const OpCode& instr)
    //     {
    //         if (instr.immediate)
    //         {
    //             AdvanceOp<Logic::op_count, Logic>::apply(this->logic, instr.mask);
    //         }
    //         else
    //             this->logic.advance_mask = instr.mask;
    //     }

    //     inline void op_Out(const OpCode& instr)
    //     {
    //         if (instr.immediate)
    //         {
    //             GatherOutput<Logic::op_count, Logic>::apply(this->logic, instr.mask);
    //         }
    //         else
    //             this->logic.gather_mask = instr.mask;
    //     }

    //     inline void op_Ldl(const OpCode& instr)
    //     {
    //         if (instr.tgt_ld < sizeof(Logic::S) / sizeof(SampleType))
    //         {
    //             SampleType *ptr = &((SampleType *)&this->logic.S)[instr.tgt_ld];
    //             *ptr = *ptr & 0xFF00 | instr.literal;
    //         }
    //     }

    //     inline void op_Ldh(const OpCode& instr)
    //     {
    //         if (instr.tgt_ld < sizeof(Logic::S) / sizeof(SampleType))
    //         {
    //             SampleType *ptr = &((SampleType *)&this->logic.S)[instr.tgt_ld];
    //             *ptr = *ptr & 0x00FF | instr.literal << 8;
    //         }
    //     }

    //     inline void op_Sel(const OpCode& instr)
    //     { 
    //         if (instr.tgt_id >= sizeof(Logic::I) / sizeof(SampleType *))
    //             return;

    //         op::IndirectIdentifier identifier = {.raw = (uint16_t)*getVar(instr.src_bank, instr.src_id) };

    //         switch (instr.sel_type)
    //         {
    //         case 0: //wave parameter
    //             if (identifier.param_id == op::output_param_id)
    //                 ((SampleType **)&this->logic.I)[instr.tgt_id] = &this->logic.output[identifier.wave_id];
    //             else 
    //                 ((SampleType **)&this->logic.I)[instr.tgt_id] = this->logic.ops.get_wave(identifier.wave_id)->getParam(identifier.param_id);
    //             break;
    //         case 1: // fixed address
    //             ((SampleType **)&this->logic.I)[instr.tgt_id] = globalLookup(identifier.buffer_id, identifier.fixed_address << 8);
    //             break;
    //         case 2: // dynamic lookup
    //         {
    //             if (instr.tgt_id >= sizeof(Logic::D) / sizeof(DynamicReference))
    //                 return;
    //             DynamicReference &d = ((DynamicReference *)&this->logic.D)[instr.tgt_id];
    //             d.index = ((SampleType **)&this->logic.I)[instr.tgt_id];
    //             d.buffer_id = identifier.buffer_id;
    //             d.shift_dir = identifier.shift_dir;
    //             d.shift_bits = identifier.shift_bits;
    //         }
    //             break;
    //         }
    //     }

    //     inline void op_Mov(const OpCode& instr)
    //     {
    //         *getVar(instr.tgt_bank, instr.tgt_id) = *getVar(instr.src_bank, instr.src_id);
    //     }

    //     inline void op_Add(const OpCode& instr)
    //     {
    //         *getVar(instr.tgt_bank, instr.tgt_id) += *getVar(instr.src_bank, instr.src_id);
    //     }

    //     inline void op_Sub(const OpCode& instr)
    //     {
    //         *getVar(instr.tgt_bank, instr.tgt_id) -= *getVar(instr.src_bank, instr.src_id);
    //     }

    //     inline void op_Mul(const OpCode& instr)
    //     {
    //         *getVar(instr.tgt_bank, instr.tgt_id) *= *getVar(instr.src_bank, instr.src_id);
    //     }

    //     inline void op_Sca(const OpCode& instr)
    //     {
    //         int32_t result = (int32_t)(*getVar(instr.tgt_bank, instr.tgt_id)) * *getVar(instr.src_bank, instr.src_id);
    //         *getVar(instr.tgt_bank, instr.tgt_id) = result >> 16;
    //     }

    //     inline void op_Lhs(const OpCode& instr)
    //     {
    //         *getVar(instr.tgt_bank, instr.tgt_id) <<= *getVar(instr.src_bank, instr.src_id);
    //     }

    //     inline void op_Rhs(const OpCode& instr)
    //     {
    //         *getVar(instr.tgt_bank, instr.tgt_id) >>= *getVar(instr.src_bank, instr.src_id);
    //     }

    //     inline void op_Ret(const OpCode& instr)
    //     {
    //         ret = *getVar(instr.src_bank, instr.src_id);
    //     }

    //     inline void op_Usr1(const OpCode& instr)
    //     {
    //         this->logic.op_Usr1(instr);
    //     }

    //     inline void op_Usr2(const OpCode& instr)
    //     {
    //         this->logic.op_Usr2(instr);
    //     }

    //     inline void op_Usr3(const OpCode& instr)
    //     {
    //         this->logic.op_Usr3(instr);
    //     }

    //     inline void exec(const OpCode &instr)
    //     {
    //         using OpFunc = void (script_wave::*)(const OpCode& instr);
    //         static constexpr OpFunc opFuncs[] = {
    //             &script_wave::op_Adv, &script_wave::op_Out, &script_wave::op_Ldl, &script_wave::op_Ldh,
    //             &script_wave::op_Sel, &script_wave::op_Mov, &script_wave::op_Add, &script_wave::op_Sub,
    //             &script_wave::op_Mul, &script_wave::op_Sca, &script_wave::op_Lhs, &script_wave::op_Rhs,
    //             &script_wave::op_Ret //still space for 3 more instructions left
    //         };
    //         (this->*opFuncs[instr.cmd])(instr);
    //     }

    //     inline void run(const OpCode *code, uint16_t size)
    //     {
    //         for (pc = 0; pc < size; pc++)
    //             exec(code[pc]);
    //     }

    //     // deep inspection
    //     void inspect(int indent = 0) override
    //     {
    //         logic_wave<Logic>::inspect(indent); // for now, just call the base class
    //         for (int i = 0; i < function::fnc_count; i++)
    //         {
    //             if (code[i])
    //             {
    //                 for (int j = 0; j < indent; ++j) printf(indent_str);
    //                 printf("  Function %d:\n", i);
    //                 for (int j = 0; j < indent; ++j) printf(indent_str);
    //                 printf("    Code (%d):\n", code_size[i]);
    //                 for (int j = 0; j < code_size[i]; j++)
    //                 {
    //                     for (int k = 0; k < indent; ++k) printf(indent_str);
    //                     printf("      %d: %s\n", j, disassemble(code[i][j]).c_str());
    //                 }
    //             }
    //         }
    //     }

    // public:
    //     const OpCode *code[function::fnc_count] = {nullptr};
    //     uint16_t code_size[function::fnc_count] = {0};
    //     SampleType ret = 0;
    //     uint16_t pc = 0;
    // };
}