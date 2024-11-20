#pragma once

#include "wave_2.h"

namespace dsp
{
    #define FOREACH_OP_0(OPERATION)  impl.ops.o0.OPERATION;
    #define FOREACH_OP_1(OPERATION)  impl.ops.o1.OPERATION; FOREACH_OP_0(OPERATION)
    #define FOREACH_OP_2(OPERATION)  impl.ops.o2.OPERATION; FOREACH_OP_1(OPERATION)
    #define FOREACH_OP_3(OPERATION)  impl.ops.o3.OPERATION; FOREACH_OP_2(OPERATION)
    #define FOREACH_OP_4(OPERATION)  impl.ops.o4.OPERATION; FOREACH_OP_3(OPERATION)
    #define FOREACH_OP_5(OPERATION)  impl.ops.o5.OPERATION; FOREACH_OP_4(OPERATION)
    #define FOREACH_OP_6(OPERATION)  impl.ops.o6.OPERATION; FOREACH_OP_5(OPERATION)
    #define FOREACH_OP_7(OPERATION)  impl.ops.o7.OPERATION; FOREACH_OP_6(OPERATION)
    #define FOREACH_OP_8(OPERATION)  impl.ops.o8.OPERATION; FOREACH_OP_7(OPERATION)
    #define FOREACH_OP_9(OPERATION)  impl.ops.o9.OPERATION; FOREACH_OP_8(OPERATION)
    #define FOREACH_OP_10(OPERATION) impl.ops.o10.OPERATION; FOREACH_OP_9(OPERATION)
    #define FOREACH_OP_11(OPERATION) impl.ops.o11.OPERATION; FOREACH_OP_10(OPERATION)
    #define FOREACH_OP_12(OPERATION) impl.ops.o12.OPERATION; FOREACH_OP_11(OPERATION)
    #define FOREACH_OP_13(OPERATION) impl.ops.o13.OPERATION; FOREACH_OP_12(OPERATION)
    #define FOREACH_OP_14(OPERATION) impl.ops.o14.OPERATION; FOREACH_OP_13(OPERATION)
    #define FOREACH_OP_15(OPERATION) impl.ops.o15.OPERATION; FOREACH_OP_14(OPERATION)
    #define FOREACH_OP_16(OPERATION) impl.ops.o16.OPERATION; FOREACH_OP_15(OPERATION)

    #define FOREACH_OP(N, OPERATION) FOREACH_OP_##N(OPERATION)
    
    #define GATHER_OUTPUT_0(mask)  if (mask & (1 << 0)) impl.output[0] = impl.ops.o0.getSample();
    #define GATHER_OUTPUT_1(mask)  GATHER_OUTPUT_0(mask)  if (mask & (1 << 1))  impl.output[1] =  impl.ops.o1.getSample();
    #define GATHER_OUTPUT_2(mask)  GATHER_OUTPUT_1(mask)  if (mask & (1 << 2))  impl.output[2] =  impl.ops.o2.getSample();
    #define GATHER_OUTPUT_3(mask)  GATHER_OUTPUT_2(mask)  if (mask & (1 << 3))  impl.output[3] =  impl.ops.o3.getSample();
    #define GATHER_OUTPUT_4(mask)  GATHER_OUTPUT_3(mask)  if (mask & (1 << 4))  impl.output[4] =  impl.ops.o4.getSample();
    #define GATHER_OUTPUT_5(mask)  GATHER_OUTPUT_4(mask)  if (mask & (1 << 5))  impl.output[5] =  impl.ops.o5.getSample();
    #define GATHER_OUTPUT_6(mask)  GATHER_OUTPUT_5(mask)  if (mask & (1 << 6))  impl.output[6] =  impl.ops.o6.getSample();
    #define GATHER_OUTPUT_7(mask)  GATHER_OUTPUT_6(mask)  if (mask & (1 << 7))  impl.output[7] =  impl.ops.o7.getSample();
    #define GATHER_OUTPUT_8(mask)  GATHER_OUTPUT_7(mask)  if (mask & (1 << 8))  impl.output[8] =  impl.ops.o8.getSample();
    #define GATHER_OUTPUT_9(mask)  GATHER_OUTPUT_8(mask)  if (mask & (1 << 9))  impl.output[9] =  impl.ops.o9.getSample();
    #define GATHER_OUTPUT_10(mask) GATHER_OUTPUT_9(mask)  if (mask & (1 << 10)) impl.output[10] = impl.ops.o10.getSample();
    #define GATHER_OUTPUT_11(mask) GATHER_OUTPUT_10(mask) if (mask & (1 << 11)) impl.output[11] = impl.ops.o11.getSample();
    #define GATHER_OUTPUT_12(mask) GATHER_OUTPUT_11(mask) if (mask & (1 << 12)) impl.output[12] = impl.ops.o12.getSample();
    #define GATHER_OUTPUT_13(mask) GATHER_OUTPUT_12(mask) if (mask & (1 << 13)) impl.output[13] = impl.ops.o13.getSample();
    #define GATHER_OUTPUT_14(mask) GATHER_OUTPUT_13(mask) if (mask & (1 << 14)) impl.output[14] = impl.ops.o14.getSample();
    #define GATHER_OUTPUT_15(mask) GATHER_OUTPUT_14(mask) if (mask & (1 << 15)) impl.output[15] = impl.ops.o15.getSample();
    #define GATHER_OUTPUT_16(mask) GATHER_OUTPUT_15(mask) if (mask & (1 << 16)) impl.output[16] = impl.ops.o16.getSample();

    #define GATHER_OUTPUT(N, mask) GATHER_OUTPUT_##N(mask)

    #define ADVANCE_OP_0(mask) if (mask & (1 << 0)) impl.o0.advance();
    #define ADVANCE_OP_1(mask)  ADVANCE_OP_0(mask)  if (mask & (1 << 1))  impl.ops.o1.advance();
    #define ADVANCE_OP_2(mask)  ADVANCE_OP_1(mask)  if (mask & (1 << 2))  impl.ops.o2.advance();
    #define ADVANCE_OP_3(mask)  ADVANCE_OP_2(mask)  if (mask & (1 << 3))  impl.ops.o3.advance();
    #define ADVANCE_OP_4(mask)  ADVANCE_OP_3(mask)  if (mask & (1 << 4))  impl.ops.o4.advance();
    #define ADVANCE_OP_5(mask)  ADVANCE_OP_4(mask)  if (mask & (1 << 5))  impl.ops.o5.advance();
    #define ADVANCE_OP_6(mask)  ADVANCE_OP_5(mask)  if (mask & (1 << 6))  impl.ops.o6.advance();
    #define ADVANCE_OP_7(mask)  ADVANCE_OP_6(mask)  if (mask & (1 << 7))  impl.ops.o7.advance();
    #define ADVANCE_OP_8(mask)  ADVANCE_OP_7(mask)  if (mask & (1 << 8))  impl.ops.o8.advance();
    #define ADVANCE_OP_9(mask)  ADVANCE_OP_8(mask)  if (mask & (1 << 9))  impl.ops.o9.advance();
    #define ADVANCE_OP_10(mask) ADVANCE_OP_9(mask)  if (mask & (1 << 10)) impl.ops.o10.advance();
    #define ADVANCE_OP_11(mask) ADVANCE_OP_10(mask) if (mask & (1 << 11)) impl.ops.o11.advance();
    #define ADVANCE_OP_12(mask) ADVANCE_OP_11(mask) if (mask & (1 << 12)) impl.ops.o12.advance();
    #define ADVANCE_OP_13(mask) ADVANCE_OP_12(mask) if (mask & (1 << 13)) impl.ops.o13.advance();
    #define ADVANCE_OP_14(mask) ADVANCE_OP_13(mask) if (mask & (1 << 14)) impl.ops.o14.advance();
    #define ADVANCE_OP_15(mask) ADVANCE_OP_14(mask) if (mask & (1 << 15)) impl.ops.o15.advance();
    #define ADVANCE_OP_16(mask) ADVANCE_OP_15(mask) if (mask & (1 << 16)) impl.ops.o16.advance();

    #define ADVANCE_OP(N, mask) ADVANCE_OP_##N(mask)

    template <typename impl>
    class wave_base : public wave
    {
    public:
        wave_base()
        {
            impl.setup();
        }
        
        // wave overload
        virtual const char *getSignature() const override { return impl.signature.data(); }

        virtual SampleType operator()() override { return getSample(); }
        virtual void operator++() override { advance(); }

        inline void gatherOutput()
        {
            GATHER_OUTPUT(impl.op_count, impl.output_mask);
        }

        inline SampleType getSample()
        {
            impl.preSampling();
            gatherOutput();
            return impl.postSampling();
        }

        inline void advanceOps()
        {
            ADVANCE_OP(impl.op_count, impl.advance_mask);
        }

        inline void advance()
        {
            impl.preAdvance();
            advanceOps();
            impl.postAdvance();
        }

        virtual inline void attack() override
        {
            FOREACH_OP(impl.op_count, attack());
        } 

        virtual inline void release() override
        {
            FOREACH_OP(impl.op_count, release());
        }

        virtual inline void stop() override
        {
            FOREACH_OP(impl.op_count, stop());
        }

        // Paramter reflection functions
        virtual int getParamCount() const override
        {
            return sizeof(impl::S) / sizeof(SampleType) + sizeof(impl::I) / sizeof(SampleType *) + sizeof(impl::D) / sizeof(DynamicReference);
        }

        virtual SampleType *getParam(uint16_t index) override
        {
            if (index < sizeof(impl::S) / sizeof(SampleType))
            {
                return &((SampleType *)&impl::S)[index];
            }
            index -= sizeof(impl::S) / sizeof(SampleType);
            if (index < sizeof(impl::I) / sizeof(SampleType *))
            {
                return ((SampleType **)&impl::I)[index];
            }
            index -= sizeof(impl::I) / sizeof(SampleType *);
            if (index < sizeof(impl::D) / sizeof(DynamicReference))
            {
                return ((DynamicReference *)&impl::D)[index]();
            }
            return nullptr;
        }

        virtual const char *getParamName(uint16_t index) const override
        {
            if (index < sizeof(impl::S) / sizeof(SampleType))
            {
                return impl::S_names[index];
            }
            index -= sizeof(impl::S) / sizeof(SampleType);
            if (index < sizeof(impl::I) / sizeof(SampleType *))
            {
                return impl::I_names[index];
            }
            index -= sizeof(impl::I) / sizeof(SampleType *);
            if (index < sizeof(impl::D) / sizeof(DynamicReference))
            {
                return impl::D_names[index];
            }
            return nullptr;
        }

        // deep inspection
        void inspect() override
        {
            wave::inspect(); // for now, just call the base class
        }
    };
}