#pragma once

#include "fpm.h"
#include "Tables_DSP.h"

#ifndef FPM_WIDTH
#define FPM_WIDTH 16
#endif

#ifndef WAVE_BUFFER_SIZE
#define WAVE_BUFFER_SIZE 1024
#endif

namespace dsp
{
    constexpr size_t BufferSize = WAVE_BUFFER_SIZE;
    using SampleDescr = ::fpm::descr<FPM_WIDTH, 0, true>;
    using SampleType = SampleDescr::t;
    using PhaseDescr = ::fpm::descr<FPM_WIDTH, -FPM_WIDTH, false>;
    using PhaseType = PhaseDescr::t;
    using flp_t = float;
    using CoeffDescr = ::fpm::descr<FPM_WIDTH, -FPM_WIDTH + 3, true>;
    // using CoeffDescr = ::fpm::descr<FPM_WIDTH*2, -(FPM_WIDTH*2) + 4, true>;
    using CoeffType = CoeffDescr::t;
    using StateDescr = ::fpm::descr<FPM_WIDTH, 0, true>;
    using StateType = StateDescr::t;
    using LevelDescr = fpm::descr<SampleDescr::width, -(SampleDescr::int_bits + SampleDescr::frac_bits), SampleDescr::is_signed>;
    using LevelType = LevelDescr::t;

    constexpr SampleType SampleMax = SampleDescr::max;
    constexpr SampleType SampleMin = SampleDescr::min;

    inline PhaseType inc_from_freq(const float frequency, const size_t sample_rate = 1)
    {
        return fpm::from_float<PhaseDescr>(frequency / sample_rate);
    }

    inline float freq_from_inc(const PhaseType increment, const size_t sample_rate = 1)
    {
        return fpm::to_float<PhaseDescr>(increment) * sample_rate;
    }

    constexpr int log2(const int n, const int p = 0)
    {
        return (n <= 1) ? p : log2(n >> 1, p + 1);
    }

    constexpr int frac_bits(const int n)
    {
        if (n <= 8)
            return 8 - n;
        if (n <= 16)
            return 16 - n;
        if (n <= 32)
            return 32 - n;
        if (n <= 64)
            return 64 - n;
    }

    template <size_t count = 5>
    void normalize(float values[count])
    {
        float sum = 0;
        for (size_t i = 0; i < count; ++i)
        {
            sum += values[i];
        }
        for (size_t i = 0; i < count; ++i)
        {
            values[i] /= sum;
        }
    }

    template<typename Target, typename Source, size_t bits = sizeof(Target) * 8>
    inline constexpr Target upperBits(const Source n)
    {
        return (Target)(n >> ((sizeof(Source)*8) - bits));
    }

    // assert that BufferSize is a power of 2
    static_assert((BufferSize > 1) && ((BufferSize & (BufferSize - 1)) == 0), "BufferSize must be a power of 2");

    // assert that table_sinWave is defined and has the same width as sampleType
    static_assert(sizeof(tables::DSP_TYPE) == sizeof(SampleType), "table_sinWave must have the same width as SampleType");
}