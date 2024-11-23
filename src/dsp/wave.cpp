#include "wave.h"
#include "wave_opCodes.h" // Add this line

namespace dsp
{
    SampleType sample_dump = 0; //pointers will point here if undefined
    SampleType *lookupBuffers[256] = {nullptr};

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
            printf("Unknown opcode string"); //crappy error handling
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

} // namespace dsp