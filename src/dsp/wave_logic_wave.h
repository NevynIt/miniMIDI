#pragma once

#include "wave_logic_base.h"

namespace dsp
{
    template<int N, typename Logic>
    struct InspectOp {
        inline static void apply(Logic& logic, int indent) {
            InspectOp<N-1, Logic>::apply(logic, indent);
            printf("%*sWave %d:\n", indent, "", N-1);
            logic.ops.template get<N-1>().inspect(indent + 1);
        }
    };

    template<typename Logic>
    struct InspectOp<0, Logic> {
        inline static void apply(Logic& logic, int indent) {
        }
    };

    template <typename Logic>
    class logic_wave : public wave
    {
    public:
        set_signature<uti::str("logic_wave"), sig_of(Logic)>;
        variant_inherit(wave)

        logic_wave()
        {
            logic.setup();
        }
        
        // wave overload
        virtual SampleType operator()() override { return getSample(); }
        virtual void operator++() override { advance(); }

        inline SampleType getSample()
        {
            return logic.getSample();
        }

        inline void advance()
        {
            logic.advance();
        }

        virtual inline void attack() override
        {
            logic.attack();
        } 

        virtual inline void release() override
        {
            logic.release();
        }

        virtual inline void stop() override
        {
            logic.stop();
        }

        // Paramter reflection functions
        virtual int getParamCount() const override
        {
            return sizeof(Logic::S) / sizeof(SampleType) + sizeof(Logic::I) / sizeof(SampleType *);
        }

        virtual SampleType *getParam(uint16_t index) override
        {
            if (index < sizeof(Logic::S) / sizeof(SampleType))
            {
                return &((SampleType *)&this->logic.S)[index];
            }
            index -= sizeof(Logic::S) / sizeof(SampleType);
            if (index < sizeof(Logic::I) / sizeof(SampleType *))
            {
                return ((SampleType **)&this->logic.I)[index];
            }
            return nullptr;
        }

        virtual const char *getParamName(uint16_t index) const override
        {
            if (index < sizeof(Logic::S) / sizeof(SampleType))
            {
                return Logic::S_names[index];
            }
            index -= sizeof(Logic::S) / sizeof(SampleType);
            if (index < sizeof(Logic::I) / sizeof(SampleType *))
            {
                return Logic::I_names[index];
            }
            return nullptr;
        }

        // deep inspection
        void inspect(int indent = 0) override
        {
            wave::inspect(indent);
            InspectOp<Logic::op_count, Logic>::apply(logic, indent);
        }

        Logic logic;
    };
}