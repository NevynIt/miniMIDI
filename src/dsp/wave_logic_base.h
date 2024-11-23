#pragma once

#include "wave_opX.h"
#include "wave_opCodes.h"

namespace dsp
{
    class DynamicReference {
    public:
        inline SampleType *operator()(bool safe = false) const
        {
            if (safe)
            {
                if (index == nullptr)
                    return &sample_dump;
                SampleType *ptr = globalLookup(buffer_id, *index);
                if (ptr == nullptr)
                    return &sample_dump;
                return ptr;
            }
            else
                return globalLookup(buffer_id, *index);
        }

        SampleType *index;
        uint8_t buffer_id;
        bool shift_dir;
        uint8_t shift_bits;        
    };

    template<typename opX = op0>
    class logic_base
    {
    public:
        logic_base() = default;
        static constexpr auto signature = uti::STR2A("impl_logic<") + opX::signature + uti::STR2A(">");
        static constexpr auto op_count = opX::op_count;

        inline void setup() // metaprogramming virtual function - "overload" in implementation
        {
        }

        inline void preSampling() // metaprogramming virtual function - "overload" in implementation
        {
        }

        inline SampleType postSampling() // metaprogramming virtual function - "overload" in implementation
        {
            if (opX::op_count)
                return output[0];
            else
                return 0;
        }

        inline void preAdvance() // metaprogramming virtual function - "overload" in implementation
        {
        }

        inline void postAdvance() // metaprogramming virtual function - "overload" in implementation
        {
        }

        inline void op_Usr1(const OpCode& instr) // metaprogramming virtual function - "overload" in implementation
        {
        }

        inline void op_Usr2(const OpCode& instr) // metaprogramming virtual function - "overload" in implementation
        {
        }

        inline void op_Usr3(const OpCode& instr) // metaprogramming virtual function - "overload" in implementation
        {
        }

        //DATA

        opX ops;
        SampleType output[opX::op_count];
        uint16_t advance_mask = 0xFF;
        uint16_t gather_mask = 0xFF;

        struct // scratchpad  // metaprogramming virtual - "overload" in implementation
        { // only SampleType variables here

        } S;
        static constexpr const char *S_names[0] = { }; // scratchpad parameter names

        struct // indirect // metaprogramming virtual - "overload" in implementation
        { // only SampleType* variables here

        } I;
        static constexpr const char *I_names[0] = { }; // indirect parameter names

        struct // dynamic lookup // metaprogramming virtual - "overload" in implementation
        { // only DynamicReference variables here

        } D;
        static constexpr const char *D_names[0] = { }; // dynamic lookup parameter names
    };
}