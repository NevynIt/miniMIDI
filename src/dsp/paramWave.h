#pragma once

#include "wave.h"

namespace dsp
{
    extern SampleType sample_dump; //pointers will point here if undefined
    extern SampleType *lookupBuffers[256];

    inline SampleType *globalLookup(uint8_t id, SampleType index)
    {
        if (lookupBuffers[id] == nullptr)
            return nullptr;
        return &lookupBuffers[id][upperBits<uint16_t, uint16_t, log2(BufferSize)>(index)];
    }

    namespace op
    {
        enum OpType : uint16_t
        {
            Adv, // set advance mask or advance immediately some waves

            Ldl, // load literal to the lower 8 bits of tgt in bank 0
            Ldh, // load literal to the higher 8 bits of tgt in bank 0

            Sel, // select target for indirect access, see below

            Mov, // tgt = src
            Add, // tgt = tgt + src
            Sub, // tgt = tgt - src
            Mul, // tgt = tgt * src
            Sca, // scale tgt = (tgt * src) >> width
            Lhs, // tgt = tgt << src
            Rhs, // tgt = tgt >> src
        };

        union OpCode
        {
            struct {
                uint16_t cmd : 4, tgt_ld : 4, literal : 8;
            };
            struct // mov, add, sub, mul, sca, lhs, rhs
            {
                uint16_t :4, tgt_bank : 2, tgt_id : 4, src_bank : 2, src_id : 4;
            };
            struct // sel
            {
                uint16_t :4, sel_type : 2, /* tgt_id */ : 4, /* src_bank */ : 2, /* src_id */ : 4; //fields same as the above after the first
            };
            struct // adv
            {
                uint16_t :4, immediate : 1, mask : 8, : 3;
            };
            uint16_t raw;
        };

        static constexpr OpCode ldl(uint8_t tgt_id, uint8_t literal)
        {
            uint16_t n = OpType::Ldl | tgt_id << 4 | literal << 8;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode ldh(uint8_t tgt_id, uint8_t literal)
        {
            uint16_t n = OpType::Ldh | tgt_id << 4 | literal << 8;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode sel(uint8_t src_type, uint8_t tgt_id, uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Sel | src_type << 4 | tgt_id << 6 | src_bank << 10 | src_id << 12;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode mov(uint8_t tgt_bank, uint8_t tgt_id, uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Mov | tgt_bank << 4 | tgt_id << 6 | src_bank << 10 | src_id << 12;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode add(uint8_t tgt_bank, uint8_t tgt_id, uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Add | tgt_bank << 4 | tgt_id << 6 | src_bank << 10 | src_id << 12;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode sub(uint8_t tgt_bank, uint8_t tgt_id, uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Sub | tgt_bank << 4 | tgt_id << 6 | src_bank << 10 | src_id << 12;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode mul(uint8_t tgt_bank, uint8_t tgt_id, uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Mul | tgt_bank << 4 | tgt_id << 6 | src_bank << 10 | src_id << 12;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode sca(uint8_t tgt_bank, uint8_t tgt_id, uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Sca | tgt_bank << 4 | tgt_id << 6 | src_bank << 10 | src_id << 12;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode lhs(uint8_t tgt_bank, uint8_t tgt_id, uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Lhs | tgt_bank << 4 | tgt_id << 6 | src_bank << 10 | src_id << 12;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode rhs(uint8_t tgt_bank, uint8_t tgt_id, uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Rhs | tgt_bank << 4 | tgt_id << 6 | src_bank << 10 | src_id << 12;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode adv(uint8_t mask, bool immediate = false)
        {
            uint16_t n = OpType::Adv | immediate << 4 | mask << 5;
            return OpCode{ .raw = n };
        }

        static constexpr uint8_t dynAccess(uint8_t scratchpad_id, uint8_t shift_dir, uint8_t shift_bits)
        {
            return scratchpad_id | shift_dir << 4 | shift_bits << 5;
        }
    }

    // indirect access can be towards a parameter of a wave within this object
    // or it can be to a point in a global buffer
    // src type
    // so: internal wave number (2 bit) + parameter number (14 bit) (-1 is the latest output of the wave)
    //     global buffer_id (8 bit) + fixed address (8 bit, scaled to the top of the scale of address)
    //     global buffer_id (8 bit) + dynamic address based on a scratchpad (4 bit) + shift direction (1 bit) + shift bits (3 bit)
    //             --> the id is going to be stored in bank0 at the same id, with information about shift operations
    //                 the address is going to be the address of where the index will be taken

    static constexpr uint16_t output_param_id = 0x1FF;

    union IndirectIdentifier
    {
        struct // src_type = 0 - wave parameter
        {
            uint16_t wave_id : 3, param_id : 13; // output_param_id is the latest output of the wave
        };
        struct // src_type = 1 - fixed address
        {
            uint16_t buffer_id : 8, fixed_address : 8;
        };
        struct // src_type = 2 - dynamic access
        {
            uint16_t /* buffer_id */ : 8, scratchpad_id : 4, shift_dir : 1, shift_bits : 3; 
            // bank1[tgt_id] will point to &bank0[scratchpad_id] initially for the dynamic index, but it can be changed with a second sel command, with src_type = 1
        };
        // src_type = 3 - reserved
        uint16_t raw;
        SampleType s;
    };

    union IndirectReference
    {
        struct
        {
            uint16_t buffer_id : 8, shift_dir : 1, shift_bits : 3, : 4;
        };
        uint16_t raw;
    };

    #define ADD_PARAM_METHODS \
        virtual int getParamCount() const override { return param_count; } \
        virtual const char *getParamName(uint16_t index) const override { if (index < param_count) return param_names[index]; return nullptr; } \
        virtual SampleType* getParam(uint16_t index) override { if (index < param_count) return getVar(param_bank[index], param_id[index]); return nullptr; }


    // I could add another parameter op_count, but it would complicate it too much to be worth it, there is not even a size impact with 8 waves, and they should be enough
    template <uint8_t scratch_size, uint8_t indirect_size, typename Base0 = wave, typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave>
    class paramWave : public wave
    {
    public:
        static constexpr auto signature = uti::STR2A("paramWave<") + Base0::signature + uti::STR2A(",") + Base1::signature + uti::STR2A(",") + Base2::signature + uti::STR2A(",") + Base3::signature + uti::STR2A(",") + Base4::signature + uti::STR2A(",") + Base5::signature + uti::STR2A(",") + Base6::signature + uti::STR2A(",") + Base7::signature + uti::STR2A(">");
        static constexpr auto param_count = 0; //number of parameters
        static constexpr const char *param_names[] = { "dummy" }; // parameter names
        static constexpr uint8_t param_bank[] = { 0 }; //banks: 0 - scratchpad, 1 - indirect, 2 - dynamic lookup, 3 - reserved
        static constexpr uint8_t param_id[] =   { 0 }; //bank and id using the logic implemented in the getVar function
        ADD_PARAM_METHODS

        //example:
        // static constexpr auto param_names = { "Param1", "Param2", "Param3", "Param4", "Param5", "Param6", "Param7", "Param8" }; 
        // static constexpr auto param_bank = { 0, 1, 0, 2, 3, 1, 2, 0 }; //banks: 0 - scratchpad, 1 - indirect, 2 - dynamic lookup, 3 - reserved
        // static constexpr auto param_id =   { 0, 0, 1, 0, 0, 1, 1, 0 }; //bank and id using the logic implemented in the getVar function

        WAVE_OPERATOR_OVERRIDE
        paramWave() 
        {
        }

        inline void setAdvanceMask(uint8_t mask)
        {
            advance_mask = mask;
        }

        void setCode(const op::OpCode *code, uint16_t size)
        {
            this->program = code;
            this->program_size = size;
        }

        inline SampleType *getVar(uint16_t bank, uint16_t id)
        {
            SampleType *ptr = nullptr;
            switch (bank)
            {
            case 0:
                ptr = &bank0[id];
                break;
            case 1:
                ptr = bank1[id];
                break;
            case 2:
            {
                SampleType *ptr = bank1[id];
                IndirectReference ref = bank2[id];
                uint16_t index = *ptr;
                if (ref.shift_dir)
                    index >>= ref.shift_bits;
                else
                    index <<= ref.shift_bits;
                ptr = globalLookup(ref.buffer_id, index);
            }
            default:
                __debugbreak();
            }
            if (ptr == nullptr)
                return &sample_dump; //pointers will point here if undefined
            return ptr;
        }

        inline void getOutputs()
        {
            output[0] = b0.getSample();
            output[1] = b1.getSample();
            output[2] = b2.getSample();
            output[3] = b3.getSample();
            output[4] = b4.getSample();
            output[5] = b5.getSample();
            output[6] = b6.getSample();
            output[7] = b7.getSample();
        }

        inline SampleType getSample()
        {
            getOutputs();
            if (program)
                run(program, program_size);
            return output[0];
        }
        
        inline void op_Adv(const op::OpCode& instr)
        {
            if (instr.immediate)
            {
                if (instr.mask & 0x01) b0.advance();
                if (instr.mask & 0x02) b1.advance();
                if (instr.mask & 0x04) b2.advance();
                if (instr.mask & 0x08) b3.advance();
                if (instr.mask & 0x10) b4.advance();
                if (instr.mask & 0x20) b5.advance();
                if (instr.mask & 0x40) b6.advance();
                if (instr.mask & 0x80) b7.advance();
            }
            else
                advance_mask = instr.mask;
        }

        inline void op_Ldl(const op::OpCode& instr)
        {
            bank0[instr.tgt_ld] = bank0[instr.tgt_ld] & 0xFF00 | instr.literal;
        }

        inline void op_Ldh(const op::OpCode& instr)
        {
            bank0[instr.tgt_ld] = bank0[instr.tgt_ld] & 0x00FF | instr.literal << 8;
        }

        inline void op_Sel(const op::OpCode& instr)
        {
            IndirectIdentifier identifier = {.raw = (uint16_t)*getVar(instr.src_bank, instr.src_id) };
            SampleType *ptr = nullptr;

            switch (instr.sel_type)
            {
            case 0: //wave parameter
                switch (identifier.wave_id)
                {
                case 0:
                    if (identifier.param_id == output_param_id)
                        ptr = &output[0];
                    else
                        ptr = b0.getParam(identifier.param_id);
                    break;
                case 1:
                    if (identifier.param_id == output_param_id)
                        ptr = &output[1];
                    else
                        ptr = b1.getParam(identifier.param_id);
                    break;
                case 2:
                    if (identifier.param_id == output_param_id)
                        ptr = &output[2];
                    else
                        ptr = b2.getParam(identifier.param_id);
                    break;
                case 3:
                    if (identifier.param_id == output_param_id)
                        ptr = &output[3];
                    else
                        ptr = b3.getParam(identifier.param_id);
                    break;
                case 4:
                    if (identifier.param_id == output_param_id)
                        ptr = &output[4];
                    else
                        ptr = b4.getParam(identifier.param_id);
                    break;
                case 5:
                    if (identifier.param_id == output_param_id)
                        ptr = &output[5];
                    else
                        ptr = b5.getParam(identifier.param_id);
                    break;
                case 6:
                    if (identifier.param_id == output_param_id)
                        ptr = &output[6];
                    else
                        ptr = b6.getParam(identifier.param_id);
                    break;
                case 7:
                    if (identifier.param_id == output_param_id)
                        ptr = &output[7];
                    else
                        ptr = b7.getParam(identifier.param_id);
                    break;
                }
                break;
            case 1: // fixed address
                ptr = globalLookup(identifier.buffer_id, identifier.fixed_address << 8);
                break;
            case 2: // dynamic lookup
                bank2[instr.tgt_id].raw = identifier.buffer_id | identifier.shift_dir << 8 | identifier.shift_bits << 9;
                ptr = &bank0[identifier.scratchpad_id];
                break;
            case 3:
                __debugbreak();
            }
            bank1[instr.tgt_id] = ptr;
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

        inline void exec(const op::OpCode &instr)
        {
            using OpFunc = void (paramWave::*)(const op::OpCode& instr);
            static constexpr OpFunc opFuncs[] = {
                &paramWave::op_Adv, &paramWave::op_Ldl, &paramWave::op_Ldh,
                &paramWave::op_Sel, &paramWave::op_Mov, &paramWave::op_Add, &paramWave::op_Sub,
                &paramWave::op_Mul, &paramWave::op_Sca, &paramWave::op_Lhs, &paramWave::op_Rhs
            };
            (this->*opFuncs[instr.cmd])(instr);
        }

        inline void run(const op::OpCode *code, uint16_t size)
        {
            for (int pc = 0; pc < size; pc++)
                exec(code[pc]);
        }

        inline void advance()
        {
            if (advance_mask & 0x01) b0.advance();
            if (advance_mask & 0x02) b1.advance();
            if (advance_mask & 0x04) b2.advance();
            if (advance_mask & 0x08) b3.advance();
            if (advance_mask & 0x10) b4.advance();
            if (advance_mask & 0x20) b5.advance();
            if (advance_mask & 0x40) b6.advance();
            if (advance_mask & 0x80) b7.advance();
        }

    public:
        Base0 b0;
        Base1 b1;
        Base2 b2;
        Base3 b3;
        Base4 b4;
        Base5 b5;
        Base6 b6;
        Base7 b7;

        //code
        const op::OpCode *program = nullptr;
        uint16_t program_size = 0;

        //registers
        uint8_t advance_mask = 0xFF;
        SampleType output[8] = {0}; // output cache
        SampleType bank0[scratch_size] = {0}; // local scratchpad
        SampleType *bank1[indirect_size] = {0}; // indirect access
        IndirectReference bank2[indirect_size] = {0}; // dynamic lookup
    };
}