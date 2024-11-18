#pragma once

#include "wave.h"

namespace dsp
{
    
    static constexpr uint8_t bank_size = 16;
    extern SampleType opModWave_global_1[bank_size]; // global scratchpad 0
    extern SampleType opModWave_global_2[bank_size]; // global scratchpad 1
    extern SampleType opModWave_dump; //pointers will point here if undefined
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
            Ret, // return contents of src
            Adv, // set advance mask or advance immediately some waves

            Ldl, // load literal to the lower 8 bits of tgt in bank 0
            Ldh, // load literal to the higher 8 bits of tgt in bank 0

            Sel, // see indirect access below

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
            struct // mov, add, sub, mul, sca, lhs, rhs, ret
            {
                uint16_t :4, tgt_bank : 2, tgt_id : 4, src_bank : 2, src_id : 4;
            };
            struct // sel
            {
                uint16_t :4, src_type : 2, /* tgt_id */ : 4, /* src_bank */ : 2, /* src_id */ : 4; //fields same as the above then
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

        static constexpr OpCode ret(uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Ret | src_bank << 10 | src_id << 12;
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

    union IndirectIdentifier
    {
        struct // src_type = 0 - wave parameter
        {
            uint16_t wave_id : 3, param_id : 13; // 0x1FF is the latest output of the wave
        };
        struct // src_type = 1 - dynamic access
        {
            uint16_t : 8, scratchpad_id : 4, shift_dir : 1, shift_bits : 3;
        };
        struct // src_type = 2 - fixed address
        {
            uint16_t buffer_id : 8, fixed_address : 8;
        };
        uint16_t raw;
    };

    union IndirectReference
    {
        struct
        {
            uint16_t buffer_id : 8, shift_dir : 1, shift_bits : 3, : 4;
        };
        SampleType raw;
    };

    template <typename Base0 = wave, typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave>
    class opModWave : public wave
    {
        static_assert(sizeof(SampleType) == 2, "SampleType must be 16 bits");

    public:
        static constexpr auto signature = uti::STR2A("opModWave<") + Base0::signature + uti::STR2A(",") + Base1::signature + uti::STR2A(",") + Base2::signature + uti::STR2A(",") + Base3::signature + uti::STR2A(",") + Base4::signature + uti::STR2A(",") + Base5::signature + uti::STR2A(",") + Base6::signature + uti::STR2A(",") + Base7::signature + uti::STR2A(">");
        WAVE_OPERATOR_OVERRIDE
        opModWave() 
        {
            reset();
        }

        void reset()
        {
            for (int i = 0; i < bank_size; i++)
            {
                bank0[i] = 0;
                bank1[i] = &bank0[i];
            }
        }

        inline void setAdvanceMask(uint8_t mask)
        {
            advance_mask = mask;
        }

        bool checkCode(const op::OpCode *code, uint16_t size)
        {
            if (code == nullptr)
                return false;

            //check that code includes a return
            for (int i = 0; i < size; i++)
            {
                if (code[i].cmd == op::OpType::Ret)
                {
                    return true;
                }
            }
            return false;
        }

        void setCode(const op::OpCode *code, uint16_t size)
        {
            if (checkCode(code, size))
            {
                this->program = code;
            }
            else
            {
                __debugbreak();
                this->program = nullptr;
            }
        }

        inline SampleType *getVar(uint16_t bank, uint16_t id)
        {
            switch (bank)
            {
            case 0:
                return &bank0[id];
                break;
            case 1:
            {
                SampleType *ptr = bank1[id];
                if (ptr >= bank0 && ptr < bank0 + bank_size)
                { //indirect access
                    IndirectReference ref = {.raw = bank0[id]};
                    uint8_t lookup_id = ref.buffer_id;
                    uint16_t index = *ptr;
                    if (ref.shift_dir)
                        index >>= ref.shift_bits;
                    else
                        index <<= ref.shift_bits;
                    return globalLookup(lookup_id, index);
                }
                else
                    return ptr;
                break;
            }
            case 2:
                return &opModWave_global_1[id];
                break;
            case 3:
                return &opModWave_global_2[id];
                break;
            default:
                __debugbreak();
                return nullptr;
            }
        }

        inline SampleType getSample()
        {
            output[0] = b0.getSample();
            output[1] = b1.getSample();
            output[2] = b2.getSample();
            output[3] = b3.getSample();
            output[4] = b4.getSample();
            output[5] = b5.getSample();
            output[6] = b6.getSample();
            output[7] = b7.getSample();
            return run();
        }

        inline SampleType run(const op::OpCode *code = nullptr, uint16_t size = 0xFFFF)
        {
            if (code == nullptr)
                if (this->program == nullptr)
                    return 0;
                else
                    code = this->program;
            else if (size == 0xFFFF)
                __debugbreak();

            pc = 0;
            
            while (pc < size)
            {
                op::OpCode instr = code[pc];
                switch (instr.cmd)
                {
                case op::OpType::Ret:
                    return *getVar(instr.src_bank, instr.src_id);
                case op::OpType::Adv:
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
                    break;
                case op::OpType::Ldl:
                    bank0[instr.tgt_ld] = bank0[instr.tgt_ld] & 0xFF00 | instr.literal;
                    break;
                case op::OpType::Ldh:
                    bank0[instr.tgt_ld] = bank0[instr.tgt_ld] & 0x00FF | instr.literal << 8;
                    break;
                case op::OpType::Sel:
                    {
                        IndirectIdentifier identifier = {.raw = (uint16_t)*getVar(instr.src_bank, instr.src_id) };
                        SampleType *ptr = nullptr;

                        switch (instr.src_type)
                        {
                        case 0: //wave parameter
                            switch (identifier.wave_id)
                            {
                            case 0:
                                if (identifier.param_id == 0x1FF)
                                    ptr = &output[0];
                                else
                                    ptr = b0.getParam(identifier.param_id);
                                break;
                            case 1:
                                if (identifier.param_id == 0x1FF)
                                    ptr = &output[1];
                                else
                                    ptr = b1.getParam(identifier.param_id);
                                break;
                            case 2:
                                if (identifier.param_id == 0x1FF)
                                    ptr = &output[2];
                                else
                                    ptr = b2.getParam(identifier.param_id);
                                break;
                            case 3:
                                if (identifier.param_id == 0x1FF)
                                    ptr = &output[3];
                                else
                                    ptr = b3.getParam(identifier.param_id);
                                break;
                            case 4:
                                if (identifier.param_id == 0x1FF)
                                    ptr = &output[4];
                                else
                                    ptr = b4.getParam(identifier.param_id);
                                break;
                            case 5:
                                if (identifier.param_id == 0x1FF)
                                    ptr = &output[5];
                                else
                                    ptr = b5.getParam(identifier.param_id);
                                break;
                            case 6:
                                if (identifier.param_id == 0x1FF)
                                    ptr = &output[6];
                                else
                                    ptr = b6.getParam(identifier.param_id);
                                break;
                            case 7:
                                if (identifier.param_id == 0x1FF)
                                    ptr = &output[7];
                                else
                                    ptr = b7.getParam(identifier.param_id);
                                break;
                            }
                            break;
                        case 1: // dynamic access
                        {
                            bank0[instr.tgt_id] = identifier.buffer_id | identifier.shift_dir << 8 | identifier.shift_bits << 9;
                            ptr = &bank0[identifier.scratchpad_id];
                            break;
                        }
                        case 2: // fixed address
                            ptr = globalLookup(identifier.buffer_id, identifier.fixed_address << 8);
                            break;
                        case 3:
                            __debugbreak();
                        }
                        if (ptr != nullptr)
                            bank1[instr.tgt_id] = ptr;
                        else
                            bank1[instr.tgt_id] = &opModWave_dump;
                    }
                    break;
                case op::OpType::Mov:
                    *getVar(instr.tgt_bank, instr.tgt_id) = *getVar(instr.src_bank, instr.src_id);
                    break;
                case op::OpType::Add:
                    *getVar(instr.tgt_bank, instr.tgt_id) += *getVar(instr.src_bank, instr.src_id);
                    break;
                case op::OpType::Sub:
                    *getVar(instr.tgt_bank, instr.tgt_id) -= *getVar(instr.src_bank, instr.src_id);
                    break;
                case op::OpType::Mul:
                    *getVar(instr.tgt_bank, instr.tgt_id) *= *getVar(instr.src_bank, instr.src_id);
                    break;
                case op::OpType::Sca:
                    {
                        int32_t result = (int32_t)(*getVar(instr.tgt_bank, instr.tgt_id)) * *getVar(instr.src_bank, instr.src_id);
                        *getVar(instr.tgt_bank, instr.tgt_id) = result >> 16;
                    }
                    break;
                case op::OpType::Lhs:
                    *getVar(instr.tgt_bank, instr.tgt_id) <<= *getVar(instr.src_bank, instr.src_id);
                    break;
                case op::OpType::Rhs:
                    *getVar(instr.tgt_bank, instr.tgt_id) >>= *getVar(instr.src_bank, instr.src_id);
                    break;
                default:
                    __debugbreak();
                }
                pc++;
            }
            __debugbreak();
            return 0;
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

        virtual int getParamCount() const override {
            int count = 8 + 16 + 16 + 3; // Updated count to include local indirect access
            if (b0.getParamCount() > 0) count += b0.getParamCount() + 1;
            if (b1.getParamCount() > 0) count += b1.getParamCount() + 1;
            if (b2.getParamCount() > 0) count += b2.getParamCount() + 1;
            if (b3.getParamCount() > 0) count += b3.getParamCount() + 1;
            if (b4.getParamCount() > 0) count += b4.getParamCount() + 1;
            if (b5.getParamCount() > 0) count += b5.getParamCount() + 1;
            if (b6.getParamCount() > 0) count += b6.getParamCount() + 1;
            if (b7.getParamCount() > 0) count += b7.getParamCount() + 1;
            return count;
        }

        virtual SampleType* getParam(uint16_t index) override {
            if (index == 0) return nullptr; // Title for outputs
            index -= 1;
            if (index < 8) {
                return &output[index];
            }
            index -= 8;
            if (index == 0) return nullptr; // Title for local scratchpad
            index -= 1;
            if (index < 16) {
                return &bank0[index];
            }
            index -= 16;
            if (index == 0) return nullptr; // Title for local indirect access
            index -= 1;
            if (index < 16) {
                return bank1[index];
            }
            index -= 16;
            if (index == 0) return nullptr; // Title for base parameters
            index -= 1;
            if (b0.getParamCount() > 0) {
                if (index == 0) return nullptr; // Title for Base0
                index -= 1;
                if (index < b0.getParamCount()) {
                    return b0.getParam(index);
                }
                index -= b0.getParamCount();
            }
            if (b1.getParamCount() > 0) {
                if (index == 0) return nullptr; // Title for Base1
                index -= 1;
                if (index < b1.getParamCount()) {
                    return b1.getParam(index);
                }
                index -= b1.getParamCount();
            }
            if (b2.getParamCount() > 0) {
                if (index == 0) return nullptr; // Title for Base2
                index -= 1;
                if (index < b2.getParamCount()) {
                    return b2.getParam(index);
                }
                index -= b2.getParamCount();
            }
            if (b3.getParamCount() > 0) {
                if (index == 0) return nullptr; // Title for Base3
                index -= 1;
                if (index < b3.getParamCount()) {
                    return b3.getParam(index);
                }
                index -= b3.getParamCount();
            }
            if (b4.getParamCount() > 0) {
                if (index == 0) return nullptr; // Title for Base4
                index -= 1;
                if (index < b4.getParamCount()) {
                    return b4.getParam(index);
                }
                index -= b4.getParamCount();
            }
            if (b5.getParamCount() > 0) {
                if (index == 0) return nullptr; // Title for Base5
                index -= 1;
                if (index < b5.getParamCount()) {
                    return b5.getParam(index);
                }
                index -= b5.getParamCount();
            }
            if (b6.getParamCount() > 0) {
                if (index == 0) return nullptr; // Title for Base6
                index -= 1;
                if (index < b6.getParamCount()) {
                    return b6.getParam(index);
                }
                index -= b6.getParamCount();
            }
            if (b7.getParamCount() > 0) {
                if (index == 0) return nullptr; // Title for Base7
                index -= 1;
                if (index < b7.getParamCount()) {
                    return b7.getParam(index);
                }
                index -= b7.getParamCount();
            }
            return nullptr;
        }

        virtual const char *getParamName(uint16_t index) const override {
            if (index == 0) return "opModWave::Output Parameters:";
            index -= 1;
            if (index < 8) {
                return "opModWave::Output";
            }
            index -= 8;
            if (index == 0) return "opModWave::Local Scratchpad Parameters:";
            index -= 1;
            if (index < 16) {
                return "opModWave::Local Scratchpad";
            }
            index -= 16;
            if (index == 0) return "opModWave::Indirect Access Parameters (as set now):";
            index -= 1;
            if (index < 16) {
                return "opModWave::Indirect Access";
            }
            index -= 16;
            if (index == 0) return "opModWave::Base Parameters:";
            index -= 1;
            if (b0.getParamCount() > 0) {
                if (index == 0) return Base0::signature.data();
                index -= 1;
                if (index < b0.getParamCount()) {
                    return b0.getParamName(index);
                }
                index -= b0.getParamCount();
            }
            if (b1.getParamCount() > 0) {
                if (index == 0) return Base1::signature.data();
                index -= 1;
                if (index < b1.getParamCount()) {
                    return b1.getParamName(index);
                }
                index -= b1.getParamCount();
            }
            if (b2.getParamCount() > 0) {
                if (index == 0) return Base2::signature.data();
                index -= 1;
                if (index < b2.getParamCount()) {
                    return b2.getParamName(index);
                }
                index -= b2.getParamCount();
            }
            if (b3.getParamCount() > 0) {
                if (index == 0) return Base3::signature.data();
                index -= 1;
                if (index < b3.getParamCount()) {
                    return b3.getParamName(index);
                }
                index -= b3.getParamCount();
            }
            if (b4.getParamCount() > 0) {
                if (index == 0) return Base4::signature.data();
                index -= 1;
                if (index < b4.getParamCount()) {
                    return b4.getParamName(index);
                }
                index -= b4.getParamCount();
            }
            if (b5.getParamCount() > 0) {
                if (index == 0) return Base5::signature.data();
                index -= 1;
                if (index < b5.getParamCount()) {
                    return b5.getParamName(index);
                }
                index -= b5.getParamCount();
            }
            if (b6.getParamCount() > 0) {
                if (index == 0) return Base6::signature.data();
                index -= 1;
                if (index < b6.getParamCount()) {
                    return b6.getParamName(index);
                }
                index -= b6.getParamCount();
            }
            if (b7.getParamCount() > 0) {
                if (index == 0) return Base7::signature.data();
                index -= 1;
                if (index < b7.getParamCount()) {
                    return b7.getParamName(index);
                }
                index -= b7.getParamCount();
            }
            return nullptr;
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
        uint8_t advance_mask = 0xFF;
        const op::OpCode *program = nullptr;
        uint16_t pc = 0;
        SampleType bank0[bank_size] = {0}; // local scratchpad
        SampleType *bank1[bank_size] = {0}; // indirect access
        SampleType output[8] = {0}; // output scratchpad
    };
}