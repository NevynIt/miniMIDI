#pragma once
#include <limits>
#include "types.h"
#include "uti.h"
#include <stdio.h>
#include <inttypes.h>

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

    static constexpr auto indent_str = "|  ";

    class wave
    {
    public:
        static constexpr auto signature = uti::STR2A(" ");
        wave() = default;
        virtual const char *getSignature() const { return signature.data(); }

        virtual SampleType operator()() { return 0; }
        virtual void operator++() { }

        inline SampleType getSample() { return this->operator()(); }
        inline void advance() { this->operator++(); }
        virtual inline void attack() { }
        virtual inline void release() { }
        virtual inline void stop() { }

        virtual int getParamCount() const { return 0; }
        virtual SampleType *getParam(uint16_t index) { return nullptr; }
        virtual const char *getParamName(uint16_t index) const { return nullptr; }

        virtual void inspect(int indent = 0)
        {
            for (int i = 0; i < indent; ++i) printf(indent_str);
            printf("%s\n", getSignature());
            for (int i = 0; i < indent; ++i) printf(indent_str);
            printf("  Params (%d):\n", getParamCount());
            for (int i = 0; i < getParamCount(); i++)
            {
                SampleType *p = getParam(i);
                SampleType v = 0;
                if (p)
                {
                    v = *p;
                    for (int j = 0; j < indent; ++j) printf(indent_str);
                    if (sizeof(SampleType) == 4)
                        printf("  - %3d (%08x) = 0x%08x -- %s\n", i, p, (uint32_t)v, getParamName(i));
                    else
                        printf("  - %3d (%08x) = 0x%04x -- %s\n", i, p, (uint16_t)v, getParamName(i));
                }
                else
                {
                    for (int j = 0; j < indent; ++j) printf(indent_str);
                    printf("  - %3d ---------------------- %s\n", i, getParamName(i));
                }
            }
        }
    };

    template <typename S = SampleType, typename srcType>
    void fillBuffer(S *buffer, srcType &src, size_t count = BufferSize)
    {
        for (size_t i = 0; i < count; ++i)
        {
            buffer[i] = upperBits<S, SampleType>(src.getSample());
            src.advance();
        }
    }

    #define WAVE_OPERATOR_OVERRIDE \
        inline SampleType operator()() override { return this->getSample(); } \
        inline void operator++() override { this->advance(); } \
        const char *getSignature() const override { return signature.data(); } \

}