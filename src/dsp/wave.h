#pragma once
#include "fpm/fixed.hpp"
#include <limits>
#include "Table_sinWave.h"

namespace dsp
{

#ifndef WAVE_SAMPLE_TYPE
#define WAVE_SAMPLE_TYPE fpm::fixed<16, 16>
#endif

#ifndef WAVE_ANGLE_TYPE
#define WAVE_ANGLE_TYPE fpm::fixed<16, 16>
#endif

#ifndef WAVE_FRAC_TYPE
#define WAVE_FRAC_TYPE fpm::fixed<16, 16>
#endif

#ifndef WAVE_BUFFER_SIZE
#define WAVE_BUFFER_SIZE 1024
#endif

    const size_t BufferSize = WAVE_BUFFER_SIZE;
    using SampleType = WAVE_SAMPLE_TYPE;
    using AngleType = WAVE_ANGLE_TYPE;
    using FracType = WAVE_FRAC_TYPE;
    const SampleType SampleMax = ::std::numeric_limits<SampleType>::max();

    // assert that SampleType is compatible with table_sinWave
    static_assert(
        (sizeof(TABLE_SINWAVE_TYPE) == sizeof(SampleType)) && (
            ((::std::is_integral<TABLE_SINWAVE_TYPE>::value) && (::std::is_integral<SampleType>::value || ::std::is_fixed_point<SampleType>::value))
            || (::std::is_floating_point<SampleType>::value && ::std::is_floating_point<TABLE_SINWAVE_TYPE>::value)
        )
        , "SampleType must be compatible with table_sinWave"
    );

    // assert that AngleType is either floating point or fpm::fixed
    static_assert(::std::is_floating_point<AngleType>::value || ::std::is_fixed_point<AngleType>::value, "AngleType must be floating point or fpm::fixed");

    // assert that FracType is either floating point or fpm::fixed
    static_assert(::std::is_floating_point<FracType>::value || ::std::is_fixed_point<FracType>::value, "FracType must be floating point or fpm::fixed");

    // assert that BufferSize is a power of 2
    static_assert((BufferSize > 1) && ((BufferSize & (BufferSize - 1)) == 0), "BufferSize must be a power of 2");

    // assert that table_sinWave is defined and has the same width as sampleType
    static_assert(sizeof(table_sinWave[0]) == sizeof(SampleType), "table_sinWave must have the same width as SampleType");

    template <typename S, typename G>
    inline S applyGain(S sample, G gain)
    {
        return sample * gain;
    }

    template <size_t I1, size_t F1, bool S1, bool R1, size_t I2, size_t F2, bool S2, bool R2>
    inline fpm::fixed<I1, F1, S1, R1> applyGain(const fpm::fixed<I1, F1, S1, R1> sample, const fpm::fixed<I2, F2, S2, R2> gain)
    {
        using IntermediateType = typename fpm::fixed<I1, F1, S1, R1>::IntermediateType;
        return fpm::fixed<I1, F1, S1>::from_raw_value(((IntermediateType)sample.raw_value() * gain.raw_value() * (S1 ? 2 : 1)) >> (I2 + F2));
    }

    template <size_t I, size_t F, bool S, bool R, typename G>
    inline fpm::fixed<I, F, S, R> applyGain(const fpm::fixed<I, F, S, R> sample, const G gain, typename ::std::enable_if<::std::is_integral<G>::value>::type * = nullptr)
    {
        constexpr size_t I2 = 8 * sizeof(G);
        using IntermediateType = typename fpm::fixed<I, F, S, R>::IntermediateType;
        return fpm::fixed<I, F, S, R>::from_raw_value(((IntermediateType)sample.raw_value() * gain * (S ? 2 : 1)) >> I2);
    }

    class wave
    {
    public:
        wave() = default;
        wave(const SampleType level) : m_level(level) {}

        // SampleType getSample() const { return m_level; }
        inline void advance() {}
        void setPhase(const AngleType phase) {}
        void setLevel(const SampleType level) { m_level = level; }
        inline SampleType getLevel() { return m_level; }

    protected:
        SampleType m_level = SampleMax;
    };

    template <typename S = SampleType, typename srcType>
    void fillBuffer(S *buffer, srcType &src, size_t count = BufferSize)
    {
        for (size_t i = 0; i < count; ++i)
        {
            buffer[i] = (S)(src.getSample());
            src.advance();
        }
    }

    template <typename T>
    inline T getRand(typename ::std::enable_if<std::is_floating_point<T>::value>::type* = nullptr)
    {
        return (T)(rand() << 1) / RAND_MAX;
    }

    template <typename T>
    inline T getRand(typename ::std::enable_if<std::is_integral<T>::value>::type* = nullptr)
    {
        return (T)(rand() << 1);
    }

    template <typename T>
    inline T getRand(typename ::std::enable_if<std::is_fixed_point<T>::value>::type* = nullptr)
    {
        return T::from_raw_value(getRand<typename T::BaseType>());
    }

    class noiseWave : public wave
    {
    public:
        noiseWave() = default;

        inline SampleType getSample() const { return applyGain(getRand<SampleType>(), this->m_level); }
    };

    class periodicWave : public wave
    {
    public:
        periodicWave() = default;
        periodicWave(const AngleType frequency, const SampleType level, const size_t sample_rate = 1) : m_increment(frequency / sample_rate), wave(level) {}

        void setFrequency(const AngleType frequency, const size_t sample_rate = 1) { m_increment = frequency / sample_rate; }
        AngleType getFrequency(const size_t sample_rate = 1) { return m_increment * sample_rate; }
        void setPhase(const AngleType phase) { m_phase = phase; }
        inline void advance() { m_phase += m_increment; }
        inline void advance(const AngleType increment, const size_t sample_rate = 1) { m_phase += increment / sample_rate; }
        AngleType getPhase() { return m_phase; }
        // SampleType getSample() const { return getSample(m_phase); }
        // SampleType getSample(const AngleType phase) const { return m_level; }

    protected:
        AngleType m_increment = AngleType(0);
        AngleType m_phase = AngleType(0);
    };

    class squareWave : public periodicWave
    {
    public:
        squareWave() = default;
        squareWave(const AngleType frequency, const SampleType level, const size_t sample_rate = 1) : periodicWave(frequency, level, sample_rate) {}

        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const AngleType phase) const
        {
            return (int)(phase * 2) & 1 ? m_level : -m_level;
        }
    };

    inline float mod1(const float x)
    {
        return fmodf(x, 1);
    }

    inline double mod1(const double x)
    {
        return fmod(x, 1);
    }

    template <size_t I, size_t F, bool S, bool R>
    inline typename fpm::fixed<I, F, S, R> mod1(const fpm::fixed<I, F, S, R> x)
    {
        return x.mod1();
    }

    class sawtoothWave : public periodicWave
    {
    public:
        sawtoothWave() = default;
        sawtoothWave(const AngleType frequency, const SampleType level, const size_t sample_rate = 1) : periodicWave(frequency, level, sample_rate) {}

        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const AngleType phase) const
        {
            return mod1(phase) * m_level * 2 - m_level;
        }
    };

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

    template <size_t count = BufferSize>
    class bufferWave : public periodicWave
    {
        static_assert((count > 1) && ((count & (count - 1)) == 0), "count must be a power of 2");

    public:
        bufferWave(const SampleType *buffer) : m_buffer(buffer) {}
        bufferWave(const SampleType *buffer, const AngleType frequency, const SampleType level, const size_t sample_rate = 1) : m_buffer(buffer) , periodicWave(frequency, level, sample_rate) {}

        void setBuffer(const SampleType *buffer)
        {
            m_buffer = buffer;
        }

        inline SampleType getSample() const { return getSample(this->m_phase); }

        template <typename AT>
        inline SampleType getSample(const AT phase, typename ::std::enable_if<std::is_floating_point<AT>::value>::type* = nullptr) const
        {
            const SampleType s = m_buffer[(phase * count) % count];
            return applyGain(s, this->m_level);
        }

        template <typename AT>
        inline SampleType getSample(const AT phase, typename ::std::enable_if<std::is_fixed_point<AT>::value>::type* = nullptr) const
        {
            constexpr static int count_bits = log2(count);
            constexpr static int count_mask = (count - 1);

            const size_t i = (phase.raw_value() / (1 << (AT::fractional_bits - count_bits))) & count_mask;
            const SampleType s = m_buffer[i];
            return applyGain(s, this->m_level);
        }

    private:
        const SampleType *m_buffer = nullptr;
    };

    class sinWave : public bufferWave<TABLE_SINWAVE_SIZE>
    {
    public:
        sinWave() : bufferWave((const SampleType *)table_sinWave) {}
        sinWave(const AngleType frequency, const SampleType level, const size_t sample_rate = 1) : bufferWave((const SampleType *)table_sinWave, frequency, level, sample_rate) {}
    };

    template <size_t harmonics = 5, typename BaseWave = sinWave>
    class harmonicWave : public BaseWave
    {
        static_assert(std::is_base_of<wave, BaseWave>::value, "BaseWave must be derived from wave");

    public:
        harmonicWave() = default;

        void setHarmonics(const FracType *harmonics_ptr)
        {
            for (size_t i = 0; i < harmonics; ++i)
            {
                m_harmonics[i] = harmonics_ptr[i];
            }
        }

        FracType getHarmonic(size_t i) const
        {
            return m_harmonics[i];
        }

        inline SampleType getSample() const { return getSample(this->m_phase); }

        inline SampleType getSample(const AngleType phase) const
        {
            SampleType sample = 0;
            for (size_t i = 0; i < harmonics; ++i)
            {
                sample += applyGain(BaseWave::getSample((i + 1) * phase), m_harmonics[i]);
            }
            return applyGain(sample, this->m_level);
        }

    private:
        FracType m_harmonics[harmonics] = {1, 0.5, 0.25, 0.125, 0.0625};
    };

    template <typename BaseWave = sinWave>
    class inharmonicWave : public BaseWave
    {
        public:
        inharmonicWave() = default;

        void setup(const FracType *ratios, const FracType *gains, size_t count)
        {
            m_count = count;
            m_ratios = ratios;
            m_gains = gains;
        }

        inline SampleType getSample() const { return getSample(this->m_phase); }

        inline SampleType getSample(const AngleType phase) const
        {
            SampleType sample = 0;
            for (size_t i = 0; i < m_count; ++i)
            {
                sample += applyGain(BaseWave::getSample(m_ratios[i] * phase), m_gains[i]);
            }
            return applyGain(sample, this->m_level);
        }

    private:
        const FracType *m_ratios = nullptr;
        const FracType *m_gains = nullptr;
        size_t m_count = 0;
    };

    class envelope
    {
    public:
        enum class State
        {
            idle,
            attack,
            decay,
            sustain,
            release
        };

        envelope() = default;

        // time in seconds
        envelope(const FracType attackRate, const FracType decayRate, const FracType sustainLevel, const FracType releaseRate)
            : m_attackRate(attackRate), m_decayRate(decayRate), m_sustainLevel(sustainLevel), m_releaseRate(releaseRate) {}

        inline void attck() { m_state = State::attack; }
        inline void decay() { m_state = State::decay; }
        inline void sustain() { m_state = State::sustain; }
        inline void release() { m_state = State::release; }
        inline void stop()
        {
            m_state = State::idle;
            m_level = FracType(0);
        }

        void setRates(const FracType attackRate, const FracType decayRate, const FracType releaseRate)
        {
            m_attackRate = attackRate;
            m_decayRate = decayRate;
            m_releaseRate = releaseRate;
        }

        void setSustainLevel(const FracType sustainLevel)
        {
            m_sustainLevel = sustainLevel;
        }

        // time in seconds
        void setTimes(const FracType attackTime, const FracType decayTime, const FracType releaseTime, const size_t sampleRate = 1)
        {
            m_attackRate = FracType(1) / (attackTime * sampleRate);
            m_decayRate = FracType(1) / (decayTime * sampleRate);
            m_releaseRate = FracType(1) / (releaseTime * sampleRate);
        }

        FracType getAttackRate() const { return m_attackRate; }
        FracType getDecayRate() const { return m_decayRate; }
        FracType getSustainLevel() const { return m_sustainLevel; }
        FracType getReleaseRate() const { return m_releaseRate; }

        inline void advance()
        {
            switch (m_state)
            {
            case State::attack:
                m_level += m_attackRate;
                if (m_level >= FracType(1))
                {
                    m_level = FracType(1);
                    m_state = State::decay;
                }
                break;
            case State::decay:
                m_level -= m_decayRate;
                if (m_level <= m_sustainLevel)
                {
                    m_level = m_sustainLevel;
                    m_state = State::sustain;
                }
                break;
            case State::release:
                m_level -= m_releaseRate;
                if (m_level <= FracType(0))
                {
                    m_level = FracType(0);
                    m_state = State::idle;
                }
                break;
            default:
                break;
            }
        }

        // between 0 and 1
        FracType getGain() const
        {
            return m_level;
        }

        State getState() const
        {
            return m_state;
        }

        void setGain(const FracType level)
        {
            m_level = level;
        }

    private:
        FracType m_attackRate = FracType(0.25);
        FracType m_decayRate = FracType(0.25);
        FracType m_sustainLevel = FracType(0.5);
        FracType m_releaseRate = FracType(0.25);
        FracType m_level = FracType(1);
        State m_state = State::idle;
    };

    template <typename BaseWave = sinWave, typename EnvelopeType = envelope>
    class signal : public BaseWave, public EnvelopeType
    {
    public:
        signal() = default;

        inline SampleType getSample() const
        {
            return applyGain(BaseWave::getSample(), EnvelopeType::getGain());
        }

        inline void advance()
        {
            BaseWave::advance();
            EnvelopeType::advance();
        }

        inline void attack() { EnvelopeType::attack(); }
        inline void decay() { EnvelopeType::decay(); }
        inline void sustain() { EnvelopeType::sustain(); }
        inline void release() { EnvelopeType::release(); }
        inline void stop() { EnvelopeType::stop(); }

    private:
        SampleType getSample(const AngleType phase) const;
    };
}