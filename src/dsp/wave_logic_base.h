#pragma once

#include "wave_opX.h"
// #include "wave_opCodes.h"
#include "exec_asm.h"

namespace dsp
{
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

        inline SampleType getSample() // metaprogramming virtual function - "overload" in implementation
        {
            return 0;
        }

        inline void advance() // metaprogramming virtual function - "overload" in implementation
        {
        }

        inline void attack() // metaprogramming virtual function - "overload" in implementation
        {
        }

        inline void release() // metaprogramming virtual function - "overload" in implementation
        {
        }

        inline void stop() // metaprogramming virtual function - "overload" in implementation
        {
        }

        static SampleType op_usr0(exec::context &ctx, const SampleType in_reg) { return in_reg; } // metaprogramming virtual function - "overload" in implementation
        static SampleType op_usr1(exec::context &ctx, const SampleType in_reg) { return in_reg; } // metaprogramming virtual function - "overload" in implementation
        static SampleType op_usr2(exec::context &ctx, const SampleType in_reg) { return in_reg; } // metaprogramming virtual function - "overload" in implementation
        static SampleType op_usr3(exec::context &ctx, const SampleType in_reg) { return in_reg; } // metaprogramming virtual function - "overload" in implementation

        //DATA
        opX ops;

        struct // scratchpad  // metaprogramming virtual - "overload" in implementation
        { // only SampleType variables here

        } S;
        static constexpr const char *S_names[0] = { }; // scratchpad parameter names

        struct // indirect // metaprogramming virtual - "overload" in implementation
        { // only SampleType* variables here

        } I;
        static constexpr const char *I_names[0] = { }; // indirect parameter names
    };
}