#pragma once

#include "wave_1.h"

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
    class wave_impl
    {
    public:
        wave_impl() = default;
        static constexpr auto signature = uti::STR2A("wave_impl<") + opX::signature + uti::STR2A(">");

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

        inline postAdvance() // metaprogramming virtual function - "overload" in implementation
        {
        }

        //DATA

        opX ops;
        SampleType output[opX::op_count];
        uint16_t advance_mask = 0;
        uint16_t gather_mask = 0;

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