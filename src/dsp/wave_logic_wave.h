#pragma once

#include "wave_logic_base.h"

namespace dsp
{
    template<int N, typename Logic>
    struct GatherOutput {
        inline static void apply(Logic& logic, int mask) {
            if (mask & (1 << N)) logic.output[N] = logic.ops.template get<N-1>().getSample();
            GatherOutput<N-1, Logic>::apply(logic, mask);
        }
    };

    template<typename Logic>
    struct GatherOutput<0, Logic> {
        inline static void apply(Logic& logic, int mask) {
            
        }
    };

    template<int N, typename Logic>
    struct AdvanceOp {
        inline static void apply(Logic& logic, int mask) {
            if (mask & (1 << N)) logic.ops.template get<N-1>().advance();
            AdvanceOp<N-1, Logic>::apply(logic, mask);
        }
    };

    template<typename Logic>
    struct AdvanceOp<0, Logic> {
        inline static void apply(Logic& logic, int mask) {
        }
    };

    template<int N, typename Logic>
    struct ForeachOp {
        inline static void apply(Logic& logic, void (wave::*operation)()) {
            (logic.ops.template get<N-1>().*operation)();
            ForeachOp<N-1, Logic>::apply(logic, operation);
        }
    };

    template<typename Logic>
    struct ForeachOp<0, Logic> {
        inline static void apply(Logic& logic, void (wave::*operation)()) {
        }
    };

    template<int N, typename Logic>
    struct InspectOp {
        inline static void apply(Logic& logic, int indent) {
            InspectOp<N-1, Logic>::apply(logic, indent);
            for (int i = 0; i < indent; ++i) std::cout << indent_str;
            std::cout << "Wave " << N-1 << ":\n";
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
        static constexpr auto signature = Logic::signature;

        logic_wave()
        {
            logic.setup();
        }
        
        // wave overload
        virtual const char *getSignature() const override { return signature.data(); }

        virtual SampleType operator()() override { return getSample(); }
        virtual void operator++() override { advance(); }

        inline void gatherOutput()
        {
            GatherOutput<Logic::op_count, Logic>::apply(logic, this->logic.gather_mask);
        }

        inline SampleType getSample()
        {
            logic.preSampling();
            gatherOutput();
            return logic.postSampling();
        }

        inline void advanceOps()
        {
            AdvanceOp<Logic::op_count, Logic>::apply(logic, this->logic.advance_mask);
        }

        inline void advance()
        {
            logic.preAdvance();
            advanceOps();
            logic.postAdvance();
        }

        virtual inline void attack() override
        {
            ForeachOp<Logic::op_count, Logic>::apply(logic, &wave::attack);
        } 

        virtual inline void release() override
        {
            ForeachOp<Logic::op_count, Logic>::apply(logic, &wave::release);
        }

        virtual inline void stop() override
        {
            ForeachOp<Logic::op_count, Logic>::apply(logic, &wave::stop);
        }

        // Paramter reflection functions
        virtual int getParamCount() const override
        {
            return sizeof(Logic::S) / sizeof(SampleType) + sizeof(Logic::I) / sizeof(SampleType *) + sizeof(Logic::D) / sizeof(DynamicReference);
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
            index -= sizeof(Logic::I) / sizeof(SampleType *);
            if (index < sizeof(Logic::D) / sizeof(DynamicReference))
            {
                return ((DynamicReference *)&this->logic.D)[index]();
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
            index -= sizeof(Logic::I) / sizeof(SampleType *);
            if (index < sizeof(Logic::D) / sizeof(DynamicReference))
            {
                return Logic::D_names[index];
            }
            return nullptr;
        }

        // deep inspection
        void inspect(int indent = 0) override
        {
            wave::inspect(indent); // for now, just call the base class
            InspectOp<Logic::op_count, Logic>::apply(logic, indent);
        }

        Logic logic;
    };
}