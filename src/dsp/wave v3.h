#pragma once
#include "fpm.h"
#include <limits>
#include "Tables_DSP.h"
#include "iir.h"

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
    constexpr SampleType SampleMin = fpm::u_min;

    // assert that BufferSize is a power of 2
    static_assert((BufferSize > 1) && ((BufferSize & (BufferSize - 1)) == 0), "BufferSize must be a power of 2");

    // assert that table_sinWave is defined and has the same width as sampleType
    static_assert(sizeof(tables::DSP_TYPE) == sizeof(SampleType), "table_sinWave must have the same width as SampleType");

    #define WAVE_OPERATOR_OVERRIDE \
        inline SampleType operator()() const override { return this->getSample(); } \
        inline void operator++() override { this->advance(); }

    //TODO: replace operator ++ with something else

    class wave
    {
    public:
        wave() = default;

        virtual SampleType operator()() const { return 0; }
        virtual void operator++() { }

        inline SampleType getSample() const { return this->operator()(); }
        inline void advance() { this->operator++(); }
    };

    class constantWave : public wave
    {
    public:
        WAVE_OPERATOR_OVERRIDE
        constantWave(const SampleType level = SampleMax) : m_level(level) {}

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
        WAVE_OPERATOR_OVERRIDE
        noiseWave() = default;

        inline SampleType getSample() const { return rand()*2; }
        inline void advance() {}
    };

    inline PhaseType inc_from_freq(const float frequency, const size_t sample_rate = 1)
    {
        return fpm::al_from_f(frequency / sample_rate);
    }

    inline float freq_from_inc(const PhaseType increment, const size_t sample_rate = 1)
    {
        return fpm::f_from_al(increment) * sample_rate;
    }

    class periodicWave : public wave
    {
    public:
        WAVE_OPERATOR_OVERRIDE
        periodicWave() = default;
        periodicWave(const PhaseType increment) : m_increment(increment) {}

        void setIncrement(const PhaseType increment) { m_increment = increment; }
        PhaseType getIncrement() { return m_increment; }        
        void setPhase(const float phase) { m_phase = fpm::al_from_f(phase); }
        inline void advance() { m_phase += m_increment; }
        inline void advance(const PhaseType increment) { m_phase += increment; }
        float getPhase() { return fpm::f_from_l(m_phase)/2; }
        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const PhaseType phase) const { return SampleMax; }

        PhaseType m_increment = PhaseType(0);
        PhaseType m_phase = PhaseType(0);
    };

    class squareWave : public periodicWave
    {
    public:
        WAVE_OPERATOR_OVERRIDE
        squareWave() = default;
        squareWave(const PhaseType increment) : periodicWave(increment) {}

        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const PhaseType phase) const
        {
            return (phase < fpm::al_from_f(0.5f)) ? SampleMax : SampleMin;
        }
    };

    class sawtoothWave : public periodicWave
    {
    public:
        WAVE_OPERATOR_OVERRIDE
        sawtoothWave() = default;
        sawtoothWave(const PhaseType increment) : periodicWave(increment) {}

        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const PhaseType phase) const
        {
            return phase;
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
        WAVE_OPERATOR_OVERRIDE
        bufferWave(const SampleType *buffer) : m_buffer(buffer) {}
        bufferWave(const SampleType *buffer, const PhaseType increment) : m_buffer(buffer) , periodicWave(increment) {}

        void setBuffer(const SampleType *buffer)
        {
            m_buffer = buffer;
        }

        inline SampleType getSample() const { return getSample(this->m_phase); }

        inline SampleType getSample(const PhaseType phase) const
        {
            constexpr static int count_bits = log2(count);

            return m_buffer[upperBits<uint16_t, PhaseType, count_bits>(phase)];
        }

        const SampleType *m_buffer = nullptr;
    };

    class builtinWave : public bufferWave<tables::DSP_SIZE>
    {
    public:
        WAVE_OPERATOR_OVERRIDE
        builtinWave() : bufferWave(tables::sinWave) {}
        builtinWave(const PhaseType increment) : bufferWave(tables::sinWave, increment) {}
        void select (const size_t i) { if (i<tables::DSP_COUNT) bufferWave::setBuffer(tables::DSP[i]); }
        void selectSin() { bufferWave::setBuffer(tables::sinWave); }
        void selectTriangle() { bufferWave::setBuffer(tables::triangleWave); }
        void selectSawtooth() { bufferWave::setBuffer(tables::sawtoothWave); }
        void selectSquare() { bufferWave::setBuffer(tables::squareWave); }
    };

    using sinWave = builtinWave;

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

    template <size_t harmonics = 5, typename Base = sinWave>
    class harmonicWave : public Base
    {
        static_assert(std::is_base_of<periodicWave, Base>::value, "Base must be derived from periodicWave");

    public:
        WAVE_OPERATOR_OVERRIDE
        harmonicWave() = default;
        harmonicWave(const PhaseType increment) : Base(increment) {}

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
                sample += fpm::u_mul_um(Base::getSample((i+1) * phase), m_harmonics[i]);
            }
            return sample;
        }

        FracType m_harmonics[harmonics] = {fpm::m_from_f(1.0/1.9), fpm::m_from_f(0.5/1.9), fpm::m_from_f(0.25/1.9), fpm::m_from_f(0.125/1.9), fpm::m_from_f(0.0625/1.9)};
    };

    template <size_t harmonics = 5, typename Base = sinWave>
    class inharmonicWave : public Base
    {
        static_assert(std::is_base_of<periodicWave, Base>::value, "Base must be derived from PeriodicWave");

        public:
        WAVE_OPERATOR_OVERRIDE
        inharmonicWave() = default;
        inharmonicWave(const PhaseType increment) : Base(increment) {}

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
                sample += fpm::u_mul_um(Base::getSample(fpm::al_mul_mal(m_ratios[i], (i+1) * phase)), m_gains[i]);
            }
            return sample;
        }

        FracType m_ratios[harmonics] = {fpm::m_from_f(1), fpm::m_from_f(2), fpm::m_from_f(3), fpm::m_from_f(4), fpm::m_from_f(5)};
        FracType m_gains[harmonics] = {fpm::m_from_f(1), fpm::m_from_f(0.5), fpm::m_from_f(0.25), fpm::m_from_f(0.125), fpm::m_from_f(0.0625)};
    };

    class envelope : public wave
    {
public:
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

        virtual inline void attack() { }
        virtual inline void decay() { }
        virtual inline void sustain() { }
        virtual inline void release() { }
        virtual inline void stop() { }
        virtual inline State getEnvState() const { return State::idle; }

        SampleType operator()() const override { return this->getEnvLevel(); }
        void operator++() override { }

        inline void advance() { this->operator++(); }

        fpm::ubase getEnvLevel() const { return 0; }
    };

    #define ENVELOPE_OPERATOR_OVERRIDE \
        inline SampleType operator()() const override { return this->getEnvLevel()/2; } \
        inline void operator++() override { this->advance(); }

    class envelopeBase : public envelope
    {
    public:
        ENVELOPE_OPERATOR_OVERRIDE
        envelopeBase() = default;

        inline void attack() override { m_state = State::attack;}
        inline void decay() override { m_state = State::decay; }
        inline void sustain() override { m_state = State::sustain; }
        inline void release() override { m_state = State::release; }
        inline void stop() override 
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

        State getEnvState() const override
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

    template <typename Base = sinWave>
    class gainModWave : public Base
    {
    public:
        WAVE_OPERATOR_OVERRIDE

        gainModWave(const SampleType level = SampleMax) : m_level(level) {}

        void setLevel(const SampleType level) { m_level = level; }
        inline SampleType getLevel() { return m_level; }

        inline SampleType getSample() const
        {
            return fpm::u_mul_ul(Base::getSample(), m_level);
        }
        inline void advance() { Base::advance(); }

        SampleType m_level = SampleMax;
    };

    template <typename Carrier = sinWave, typename Modulator = envelopeBase>
    class amModWave : public wave
    {
    public:
        WAVE_OPERATOR_OVERRIDE
        amModWave() = default;

        inline SampleType getSample() const
        {
            const SampleType level = m.getSample();
            return fpm::u_mul_ul(c.getSample(), level);
        }
        inline void advance() { c.advance(); m.advance(); }

    public:
        Carrier c;
        Modulator m;
    };

    template <typename Carrier = sinWave, typename Modulator = sinWave>
    class pmModWave : public wave
    {
    public:
        WAVE_OPERATOR_OVERRIDE
        pmModWave() = default;

        inline SampleType getSample() const
        {
            SampleType level = m.getSample();
            c.advance((PhaseType)level); //apply the phase modulation temporarily
            SampleType s = c.getSample();
            c.advance((PhaseType)-level); //restore the phase
            return s;
        }

        inline void advance()
        {
            m.advance();
            c.advance(); //Advance by its own phase
        }

    public:
        Carrier c;
        Modulator m;
    };

    template <typename Carrier = sinWave, typename Modulator = sinWave>
    class fmModWave : public wave
    {
    public:
        WAVE_OPERATOR_OVERRIDE
        fmModWave() = default;

        inline SampleType getSample() const
        {
            return c.getSample();
        }

        inline void advance()
        {
            SampleType level = m.getSample();
            m.advance();
            c.advance(); //Advance by its own phase first
            c.advance((PhaseType)level); //then apply the frequency modulation
        }
    
    public:
        Carrier c;
        Modulator m;
    };

    template <typename Base = sawtoothWave>
    class RBJFilterWave : public Base, public iir::RBJ
    {
    public:
        WAVE_OPERATOR_OVERRIDE
        RBJFilterWave() = default;

        inline SampleType getSample() const
        {
            return cache;
        }

        inline void advance()
        {
            cache = RBJ::filter(Base::getSample());
            Base::advance();
        }

    public:
        SampleType cache = 0;
    };
}