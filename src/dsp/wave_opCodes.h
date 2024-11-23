#pragma once

#include "wave_root.h"
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector> // Add this line

namespace dsp
{
    enum OpType : uint16_t
    {

        Adv, // set advance mask or advance immediately some waves
        Out, // set the gather mask or gather the outputs of some waves in the object

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

        Ret, // set the return value to the contents of src

        Usr1, // user defined 1
        Usr2, // user defined 2
        Usr3, // user defined 3
    };

    typedef union // note: only the first 16 parameters per bank are accessible
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
            uint16_t :4, sel_type : 2, /* tgt_id */ : 4, /* src_bank */ : 2, /* src_id */ : 4; //fields same as the above after the first
        };
        struct // adv, out
        {
            uint16_t :4, immediate : 1, mask : 11;
        };
        struct // usr1, usr2, usr3
        {
            uint16_t :4, param : 12;
        };
        uint16_t raw;
    } OpCode;

    namespace op
    {
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

        static constexpr OpCode out(uint8_t mask, bool immediate = false)
        {
            uint16_t n = OpType::Out | immediate << 4 | mask << 5;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode ret(uint8_t src_bank, uint8_t src_id)
        {
            uint16_t n = OpType::Ret | src_bank << 10 | src_id << 12;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode usr1(uint16_t param)
        {
            uint16_t n = OpType::Usr1 | param << 4;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode usr2(uint16_t param)
        {
            uint16_t n = OpType::Usr2 | param << 4;
            return OpCode{ .raw = n };
        }

        static constexpr OpCode usr3(uint16_t param)
        {
            uint16_t n = OpType::Usr3 | param << 4;
            return OpCode{ .raw = n };
        }

        static constexpr uint8_t dynAccess(uint8_t scratchpad_id, uint8_t shift_dir, uint8_t shift_bits)
        {
            return scratchpad_id | shift_dir << 4 | shift_bits << 5;
        }
 
        static constexpr uint16_t output_param_id = 0x1FF;

        union IndirectIdentifier // used with the Sel opcode 
        {
            struct // src_type = 0 - wave parameter - only the first 8 ops are accessible
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
    }

    std::string disassemble(OpCode opcode)
    {
        std::ostringstream oss;
        switch (opcode.cmd)
        {
        case OpType::Adv:
            oss << "Adv " << opcode.mask << (opcode.immediate ? " immediate" : "");
            break;
        case OpType::Out:
            oss << "Out " << opcode.mask << (opcode.immediate ? " immediate" : "");
            break;
        case OpType::Ldl:
            oss << "Ldl " << opcode.tgt_ld << " " << opcode.literal;
            break;
        case OpType::Ldh:
            oss << "Ldh " << opcode.tgt_ld << " " << opcode.literal;
            break;
        case OpType::Sel:
            oss << "Sel " << opcode.sel_type << " " << opcode.tgt_id << " " << opcode.src_bank << " " << opcode.src_id;
            break;
        case OpType::Mov:
            oss << "Mov " << opcode.tgt_bank << " " << opcode.tgt_id << " " << opcode.src_bank << " " << opcode.src_id;
            break;
        case OpType::Add:
            oss << "Add " << opcode.tgt_bank << " " << opcode.tgt_id << " " << opcode.src_bank << " " << opcode.src_id;
            break;
        case OpType::Sub:
            oss << "Sub " << opcode.tgt_bank << " " << opcode.tgt_id << " " << opcode.src_bank << " " << opcode.src_id;
            break;
        case OpType::Mul:
            oss << "Mul " << opcode.tgt_bank << " " << opcode.tgt_id << " " << opcode.src_bank << " " << opcode.src_id;
            break;
        case OpType::Sca:
            oss << "Sca " << opcode.tgt_bank << " " << opcode.tgt_id << " " << opcode.src_bank << " " << opcode.src_id;
            break;
        case OpType::Lhs:
            oss << "Lhs " << opcode.tgt_bank << " " << opcode.tgt_id << " " << opcode.src_bank << " " << opcode.src_id;
            break;
        case OpType::Rhs:
            oss << "Rhs " << opcode.tgt_bank << " " << opcode.tgt_id << " " << opcode.src_bank << " " << opcode.src_id;
            break;
        case OpType::Ret:
            oss << "Ret " << opcode.src_bank << " " << opcode.src_id;
            break;
        case OpType::Usr1:
            oss << "Usr1 " << opcode.param;
            break;
        case OpType::Usr2:
            oss << "Usr2 " << opcode.param;
            break;
        case OpType::Usr3:
            oss << "Usr3 " << opcode.param;
            break;
        default:
            oss << "Unknown opcode";
            break;
        }
        return oss.str();
    }

    OpCode assemble_one(const std::string& str)
    {
        std::istringstream iss(str);
        std::string op;
        iss >> op;

        static const std::unordered_map<std::string, OpType> opMap = {
            {"Adv", OpType::Adv}, {"Out", OpType::Out}, {"Ldl", OpType::Ldl}, {"Ldh", OpType::Ldh},
            {"Sel", OpType::Sel}, {"Mov", OpType::Mov}, {"Add", OpType::Add}, {"Sub", OpType::Sub},
            {"Mul", OpType::Mul}, {"Sca", OpType::Sca}, {"Lhs", OpType::Lhs}, {"Rhs", OpType::Rhs},
            {"Ret", OpType::Ret}, {"Usr1", OpType::Usr1}, {"Usr2", OpType::Usr2}, {"Usr3", OpType::Usr3}
        };

        OpCode opcode = {};
        opcode.cmd = opMap.at(op);

        switch (opcode.cmd)
        {
        case OpType::Adv:
        case OpType::Out:
        {
            uint16_t mask;
            iss >> std::skipws >> mask;
            opcode.mask = mask;
            std::string immediate;
            if (iss >> immediate && immediate == "immediate")
            {
                opcode.immediate = true;
            }
            break;
        }
        case OpType::Ldl:
        case OpType::Ldh:
        {
            uint16_t tgt_ld, literal;
            iss >> std::skipws >> tgt_ld >> literal;
            opcode.tgt_ld = tgt_ld;
            opcode.literal = literal;
            break;
        }
        case OpType::Sel:
        {
            uint16_t sel_type, tgt_id, src_bank, src_id;
            iss >> std::skipws >> sel_type >> tgt_id >> src_bank >> src_id;
            opcode.sel_type = sel_type;
            opcode.tgt_id = tgt_id;
            opcode.src_bank = src_bank;
            opcode.src_id = src_id;
            break;
        }
        case OpType::Mov:
        case OpType::Add:
        case OpType::Sub:
        case OpType::Mul:
        case OpType::Sca:
        case OpType::Lhs:
        case OpType::Rhs:
        {
            uint16_t tgt_bank, tgt_id, src_bank, src_id;
            iss >> std::skipws >> tgt_bank >> tgt_id >> src_bank >> src_id;
            opcode.tgt_bank = tgt_bank;
            opcode.tgt_id = tgt_id;
            opcode.src_bank = src_bank;
            opcode.src_id = src_id;
            break;
        }
        case OpType::Ret:
        {
            uint16_t src_bank, src_id;
            iss >> std::skipws >> src_bank >> src_id;
            opcode.src_bank = src_bank;
            opcode.src_id = src_id;
            break;
        }
        case OpType::Usr1:
        case OpType::Usr2:
        case OpType::Usr3:
        {
            uint16_t param;
            iss >> std::skipws >> param;
            opcode.param = param;
            break;
        }
        default:
            throw std::invalid_argument("Unknown opcode string");
        }

        return opcode;
    }

    std::vector<OpCode> assemble(const std::string& str)
    {
        std::vector<OpCode> opcodes;
        std::istringstream iss(str);
        std::string line;

        while (std::getline(iss, line))
        {
            // Trim the line
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            // Skip lines starting with #
            if (line.empty() || line[0] == '#')
            {
                continue;
            }

            // Assemble the opcode
            opcodes.push_back(assemble_one(line));
        }

        return opcodes;
    }
}