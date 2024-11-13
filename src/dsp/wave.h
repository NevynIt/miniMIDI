#pragma once
#include "fpm.h"
#include <limits>
#include "Table_sinWave.h"

namespace dsp
{

#ifndef WAVE_BUFFER_SIZE
#define WAVE_BUFFER_SIZE 1024
#endif

    constexpr size_t BufferSize = WAVE_BUFFER_SIZE;
    using SampleType = fpm::base; //upper bits
    using FracType = fpm::base; //middle bits
    using PhaseType = fpm::ubase; //lower bits
    constexpr SampleType SampleMax = fpm::b_max;

    // assert that BufferSize is a power of 2
    static_assert((BufferSize > 1) && ((BufferSize & (BufferSize - 1)) == 0), "BufferSize must be a power of 2");

    // assert that table_sinWave is defined and has the same width as sampleType
    static_assert(sizeof(table_sinWave[0]) == sizeof(SampleType), "table_sinWave must have the same width as SampleType");

    class wave
    {
    public:
        wave() = default;
        wave(const SampleType level) : m_level(level) {}

        inline SampleType getSample() const { return m_level; }
        inline void advance() {}
        void setLevel(const SampleType level) { m_level = level; }
        inline SampleType getLevel() { return m_level; }

        SampleType m_level = SampleMax;
    };

    template<typename Target, typename Source, size_t bits = sizeof(Target) * 8>
    inline constexpr Target upperBits(const Source n)
    {
        return (Target)(n >> ((sizeof(Source)*8) - bits));
    }

    template <typename S = SampleType, typename srcType>
    void fillBuffer(S *buffer, srcType &src, size_t count = BufferSize)
    {
        for (size_t i = 0; i < count; ++i)
        {
            buffer[i] = upperBits<S, SampleType>(src.getSample());
            src.advance();
        }
    }

    class noiseWave : public wave
    {
    public:
        noiseWave() = default;

        inline SampleType getSample() const { return fpm::u_mul_ul((SampleType)rand()*2, m_level); }
    };

    class periodicWave : public wave
    {
    public:
        periodicWave() = default;
        periodicWave(const float frequency, const SampleType level, const size_t sample_rate = 1) : wave(level) { setFrequency(frequency, sample_rate); }

        void setFrequency(const float frequency, const size_t sample_rate = 1) { m_increment = fpm::al_from_f(frequency / sample_rate); }
        float getFrequency(const size_t sample_rate = 1) { return fpm::f_from_al(m_increment) * sample_rate; }
        void setIncrement(const PhaseType increment) { m_increment = increment; }
        PhaseType getIncrement() { return m_increment; }        
        void setPhase(const float phase) { m_phase = fpm::al_from_f(phase); }
        inline void advance() { m_phase += m_increment; }
        inline void advance(const float increment, const size_t sample_rate = 1) { m_phase += fpm::l_from_f(increment / sample_rate * 2); }
        float getPhase() { return fpm::f_from_l(m_phase)/2; }
        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const PhaseType phase) const { return m_level; }

        PhaseType m_increment = PhaseType(0);
        PhaseType m_phase = PhaseType(0);
    };

    class squareWave : public periodicWave
    {
    public:
        squareWave() = default;
        squareWave(const float frequency, const SampleType level, const size_t sample_rate = 1) : periodicWave(frequency, level, sample_rate) {}

        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const PhaseType phase) const
        {
            return (phase < fpm::al_from_f(0.5f)) ? m_level : -m_level;
        }
    };

    class sawtoothWave : public periodicWave
    {
    public:
        sawtoothWave() = default;
        sawtoothWave(const float frequency, const SampleType level, const size_t sample_rate = 1) : periodicWave(frequency, level, sample_rate) {}

        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const PhaseType phase) const
        {
            return fpm::u_mul_ual(m_level, phase) * 2;
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
        bufferWave(const SampleType *buffer, const float frequency, const SampleType level, const size_t sample_rate = 1) : m_buffer(buffer) , periodicWave(frequency, level, sample_rate) {}

        void setBuffer(const SampleType *buffer)
        {
            m_buffer = buffer;
        }

        inline SampleType getSample() const { return getSample(this->m_phase); }

        inline SampleType getSample(const PhaseType phase) const
        {
            constexpr static int count_bits = log2(count);

            const SampleType s = m_buffer[upperBits<uint16_t, PhaseType, count_bits>(phase)];
            return fpm::u_mul_ul(s, this->m_level);
        }

    private:
        const SampleType *m_buffer = nullptr;
    };

    class sinWave : public bufferWave<TABLE_SINWAVE_SIZE>
    {
    public:
        sinWave() : bufferWave(table_sinWave) {}
        sinWave(const float frequency, const SampleType level, const size_t sample_rate = 1) : bufferWave(table_sinWave, frequency, level, sample_rate) {}
    };

    template <size_t harmonics = 5, typename BaseWave = sinWave>
    class harmonicWave : public BaseWave
    {
        static_assert(std::is_base_of<periodicWave, BaseWave>::value, "BaseWave must be derived from periodicWave");

    public:
        harmonicWave() = default;
        harmonicWave(const float frequency, const SampleType level, const size_t sample_rate = 1) : BaseWave(frequency, level, sample_rate) {}

        void setHarmonics(const float harmonics_f[harmonics])
        {
            for (size_t i = 0; i < harmonics; ++i)
            {
                m_harmonics[i] = fpm::m_from_f(harmonics_f[i]);
            }
        }

        FracType getHarmonic(size_t i) const
        {
            return m_harmonics[i];
        }

        inline SampleType getSample() const { return getSample(this->m_phase); }

        inline SampleType getSample(const PhaseType phase) const
        {
            SampleType sample = 0;
            for (size_t i = 0; i < harmonics; ++i)
            {
                sample += fpm::u_mul_um(BaseWave::getSample((i+1) * phase), m_harmonics[i]); //samples are already scaled by level
            }
            return sample;
        }

    public:
        FracType m_harmonics[harmonics] = {fpm::m_from_f(1), fpm::m_from_f(0.5), fpm::m_from_f(0.25), fpm::m_from_f(0.125), fpm::m_from_f(0.0625)};
    };

    template <size_t harmonics = 5, typename BaseWave = sinWave>
    class inharmonicWave : public BaseWave
    {
        static_assert(std::is_base_of<periodicWave, BaseWave>::value, "BaseWave must be derived from wave");

        public:
        inharmonicWave() = default;
        inharmonicWave(const float frequency, const SampleType level, const size_t sample_rate = 1) : BaseWave(frequency, level, sample_rate) {}

        void setup(const float ratios[harmonics], const float gains[harmonics])
        {
            for (size_t i = 0; i < harmonics; ++i)
            {
                m_ratios[i] = fpm::m_from_f(ratios[i]);
                m_gains[i] = fpm::m_from_f(gains[i]);
            }
        }

        inline SampleType getSample() const { return getSample(this->m_phase); }

        inline SampleType getSample(const PhaseType phase) const
        {
            SampleType sample = 0;
            for (size_t i = 0; i < harmonics; ++i)
            {
                sample += fpm::u_mul_um(BaseWave::getSample(fpm::al_mul_mal(m_ratios[i], (i+1) * phase)), m_gains[i]);
            }
            return sample;
        }

    private:
        FracType m_ratios[harmonics] = {fpm::m_from_f(1), fpm::m_from_f(2), fpm::m_from_f(3), fpm::m_from_f(4), fpm::m_from_f(5)};
        FracType m_gains[harmonics] = {fpm::m_from_f(1), fpm::m_from_f(0.5), fpm::m_from_f(0.25), fpm::m_from_f(0.125), fpm::m_from_f(0.0625)};
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
            m_level = 0;
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
            m_attackRate = fpm::m_one / (attackTime * sampleRate);
            m_decayRate = fpm::m_one / (decayTime * sampleRate);
            m_releaseRate = fpm::m_one / (releaseTime * sampleRate);
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
        FracType getLevel() const
        {
            return m_level;
        }

        State getState() const
        {
            return m_state;
        }

        void setLevel(const FracType level)
        {
            m_level = level;
        }

    private:
        FracType m_attackRate = fpm::m_from_f(0.25);
        FracType m_decayRate = fpm::m_from_f(0.25);
        FracType m_sustainLevel = fpm::m_from_f(0.5);
        FracType m_releaseRate = fpm::m_from_f(0.25);
        FracType m_level = fpm::m_from_f(0);
        State m_state = State::idle;
    };

    template <typename BaseWave = sinWave, typename EnvelopeType = envelope>
    class signal : public BaseWave, public EnvelopeType
    {
    public:
        signal() = default;

        inline SampleType getSample() const
        {
            return fpm::u_mul_um(BaseWave::getSample(), EnvelopeType::getLevel());
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
        SampleType getSample(const PhaseType phase) const;
    };
}