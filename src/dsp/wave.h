#pragma once
#include "fpm.h"
#include <limits>
#include "Tables_DSP.h"

namespace dsp
{

#ifndef WAVE_BUFFER_SIZE
#define WAVE_BUFFER_SIZE 1024
#endif

    constexpr size_t BufferSize = WAVE_BUFFER_SIZE;
    using SampleType = fpm::u_type; //upper bits
    using FracType = fpm::m_type; //middle bits
    using PhaseType = fpm::al_type; //lower bits
    constexpr SampleType SampleMax = fpm::u_max;

    // assert that BufferSize is a power of 2
    static_assert((BufferSize > 1) && ((BufferSize & (BufferSize - 1)) == 0), "BufferSize must be a power of 2");

    // assert that table_sinWave is defined and has the same width as sampleType
    static_assert(sizeof(tables::DSP_TYPE) == sizeof(SampleType), "table_sinWave must have the same width as SampleType");

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

        const SampleType *m_buffer = nullptr;
    };

    class builtinWave : public bufferWave<tables::DSP_SIZE>
    {
    public:
        builtinWave() : bufferWave(tables::sinWave) {}
        builtinWave(const float frequency, const SampleType level, const size_t sample_rate = 1) : bufferWave(tables::sinWave, frequency, level, sample_rate) {}
        void select (const size_t i) { if (i<tables::DSP_COUNT) bufferWave::setBuffer(tables::DSP[i]); }
        void selectSin() { bufferWave::setBuffer(tables::sinWave); }
        void selectTriangle() { bufferWave::setBuffer(tables::triangleWave); }
        void selectSawtooth() { bufferWave::setBuffer(tables::sawtoothWave); }
        void selectSquare() { bufferWave::setBuffer(tables::squareWave); }
    };

    using sinWave = builtinWave;

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

        FracType m_harmonics[harmonics] = {fpm::m_from_f(1.0/1.9), fpm::m_from_f(0.5/1.9), fpm::m_from_f(0.25/1.9), fpm::m_from_f(0.125/1.9), fpm::m_from_f(0.0625/1.9)};
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

        inline void attack() { m_state = State::attack;}
        inline void decay() { m_state = State::decay; }
        inline void sustain() { m_state = State::sustain; }
        inline void release() { m_state = State::release; }
        inline void stop()
        {
            m_state = State::idle;
            m_level = 0;
        }

        void setEnvRates(const FracType attackRate, const FracType decayRate, const FracType releaseRate)
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
        void setEnvTimes(const float attackTime, const float decayTime, const float releaseTime, const size_t sampleRate = 1)
        {
            m_attackRate = fpm::au_max / (attackTime * sampleRate);
            m_decayRate = fpm::au_max / (decayTime * sampleRate);
            m_releaseRate = fpm::au_max / (releaseTime * sampleRate);
        }

        inline void advance()
        {
            switch (m_state)
            {
            case State::attack:
                if (fpm::au_max - m_level < m_attackRate)
                {
                    m_level = fpm::au_max;
                    m_state = State::decay;
                }
                else
                {
                    m_level += m_attackRate;
                }
                break;
            case State::decay:
                if (m_level < m_sustainLevel + m_decayRate)
                {
                    m_level = m_sustainLevel;
                    m_state = State::sustain;
                }
                else
                {
                    m_level -= m_decayRate;
                }
                break;
            case State::release:
                if (m_level < m_releaseRate)
                {
                    m_level = 0;
                    m_state = State::idle;
                }
                else
                {
                    m_level -= m_releaseRate;
                }
                break;
            default:
                break;
            }
        }

        // between 0 and fpm::ab_max
        fpm::ubase getEnvLevel() const
        {
            return m_level;
        }

        State getEnvState() const
        {
            return m_state;
        }

        fpm::au_type m_attackRate = (fpm::au_max / 2)/100;
        fpm::au_type m_decayRate = (fpm::au_max / 4)/100;
        fpm::au_type m_sustainLevel = fpm::au_max / 2;
        fpm::au_type m_releaseRate = (fpm::au_max / 4)/100;
        fpm::au_type m_level = 0;
        State m_state = State::idle;
    };

//next step envelope that takes the level from a buffer

    template <typename BaseWave = sinWave, typename EnvelopeType = envelope>
    class signal : public BaseWave, public EnvelopeType
    {
    public:
        signal() = default;

        inline SampleType getSample() const
        {
            return fpm::u_mul_ual(BaseWave::getSample(), EnvelopeType::getEnvLevel());
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