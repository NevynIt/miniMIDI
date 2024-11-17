#pragma once
#include <limits>
#include "types.h"
#include "iir.h"
#include "uti.h"
#include <stdio.h>

namespace dsp
{
    #define WAVE_OPERATOR_OVERRIDE \
        inline SampleType operator()() const override { return this->getSample(); } \
        inline void operator++() override { this->advance(); } \
        const char *getSignature() const override { return signature.data(); } \

    //TODO: replace operator ++ with something else

    class wave
    {
    public:
        static constexpr auto signature = uti::to_array("wave");
        wave() = default;
        virtual const char *getSignature() const { return signature.data(); }

        virtual SampleType operator()() const { return 0; }
        virtual void operator++() { }

        inline SampleType getSample() const { return this->operator()(); }
        inline void advance() { this->operator++(); }

        virtual int getParamCount() const { return 0; }
        virtual SampleType *getParam(int index) { return nullptr; }
        virtual const char *getParamName(int index) const { return nullptr; }

        void inspect()
        {
            printf("%s\n", getSignature());
            printf("  Params (%d):\n", getParamCount());
            for (int i = 0; i < getParamCount(); i++)
            {
                SampleType *p = getParam(i);
                SampleType v = 0;
                if (p)
                    v = *p;
                printf("  - %3d (%p) = 0x%08x #%s\n", i, p, v, getParamName(i));
            }
        }
    };

    class constantWave : public wave
    {
    public:
        static constexpr auto signature = uti::to_array("constantWave");
        WAVE_OPERATOR_OVERRIDE
        constantWave(const SampleType level = SampleMax) : m_level(level) {}

        inline SampleType getSample() const { return m_level; }
        inline void advance() {}
        void setLevel(const SampleType level) { m_level = level; }
        inline SampleType getLevel() { return m_level; }

        SampleType m_level = SampleMax;

        virtual int getParamCount() const { return 1; }
        virtual SampleType* getParam(int index) 
        {
            if (index==0) return &m_level;
            return nullptr;
        }
        virtual const char *getParamName(int index) const
        {
            if (index==0) return "constantWave::Level";
            return nullptr;
        }
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
        static constexpr auto signature = uti::to_array("noiseWave");
        WAVE_OPERATOR_OVERRIDE
        noiseWave() = default;

        inline SampleType getSample() const { return rand()*2; }
        inline void advance() {}
    };

    inline PhaseType inc_from_freq(const float frequency, const size_t sample_rate = 1)
    {
        return fpm::from_float<PhaseDescr>(frequency / sample_rate);
    }

    inline float freq_from_inc(const PhaseType increment, const size_t sample_rate = 1)
    {
        return fpm::to_float<PhaseDescr>(increment) * sample_rate;
    }

    class periodicWave : public wave
    {
    public:
        static constexpr auto signature = uti::to_array("periodicWave");
        WAVE_OPERATOR_OVERRIDE
        periodicWave() = default;
        periodicWave(const PhaseType increment) : m_increment(increment) {}

        void setIncrement(const PhaseType increment) { m_increment = increment; }
        PhaseType getIncrement() { return m_increment; }        
        void setPhase(const PhaseType phase) { m_phase = phase; }
        inline void advance() { m_phase += m_increment; }
        inline void advance(const PhaseType increment) { m_phase += increment; }
        PhaseType getPhase() { return m_phase; }
        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const PhaseType phase) const { return 0; }

        PhaseType m_increment = 0;
        PhaseType m_phase = 0;

        virtual int getParamCount() const  override { return 2; }
        virtual SampleType* getParam(int index) override
        {
            if (index==0) return (SampleType *)&m_increment;
            if (index==1) return (SampleType *)&m_phase;
            return nullptr;
        }
        virtual const char *getParamName(int index) const override
        {
            if (index==0) return "periodicWave::Increment";
            if (index==1) return "periodicWave::Phase";
            return nullptr;
        }
    };

    class squareWave : public periodicWave
    {
    public:
        static constexpr auto signature = uti::to_array("squareWave");
        WAVE_OPERATOR_OVERRIDE
        squareWave() = default;
        squareWave(const PhaseType increment) : periodicWave(increment) {}

        inline SampleType getSample() const { return getSample(m_phase); }
        inline SampleType getSample(const PhaseType phase) const
        {
            return (phase < fpm::from_float<PhaseDescr>(0.5f)) ? SampleMax : SampleMin;
        }
    };

    class sawtoothWave : public periodicWave
    {
    public:
        static constexpr auto signature = uti::to_array("sawtoothWave");
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

    template <int count = BufferSize>
    class bufferWave : public periodicWave
    {
        static_assert((count > 1) && ((count & (count - 1)) == 0), "count must be a power of 2");

    public:
        static constexpr auto signature = uti::concatenate(uti::concatenate(uti::to_array("bufferWave<"), uti::int_to_string<count>()),uti::to_array(">"));
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
        static constexpr auto signature = uti::to_array("builtinWave");
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
        static constexpr auto signature = uti::concatenate(uti::concatenate(uti::to_array("harmonicWave<"), Base::signature),uti::to_array(">"));
        WAVE_OPERATOR_OVERRIDE
        harmonicWave() = default;
        harmonicWave(const PhaseType increment) : Base(increment) {}

        void setHarmonics(const float harmonics_f[harmonics])
        {
            for (size_t i = 0; i < harmonics; ++i)
            {
                m_harmonics[i] = fpm::from_float<CoeffDescr>(harmonics_f[i]);
            }
        }

        CoeffDescr getHarmonic(size_t i) const
        {
            return m_harmonics[i];
        }

        inline SampleType getSample() const { return getSample(this->m_phase); }

        inline SampleType getSample(const PhaseType phase) const
        {
            SampleType sample = 0;
            for (size_t i = 0; i < harmonics; ++i)
            {
                sample += fpm::mul<SampleDescr,SampleDescr,CoeffDescr>(Base::getSample((i+1) * phase), m_harmonics[i]);
            }
            return sample;
        }

        CoeffType m_harmonics[harmonics] = {0};

        virtual int getParamCount() const  override { return harmonics + Base::getParamCount(); }
        virtual SampleType* getParam(int index)  override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParam(index);
            }
            index -= baseCount;
            if (index < harmonics) return (SampleType *)&m_harmonics[index];
            return nullptr;
        }
        virtual const char *getParamName(int index) const override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParamName(index);
            }
            index -= baseCount;
            if (index < harmonics) return "harmonicWave::Formant_Gain";
            return nullptr;
        }
    };


    template <size_t harmonics = 5, typename Base = sinWave>
    class inharmonicWave : public Base
    {
        static_assert(std::is_base_of<periodicWave, Base>::value, "Base must be derived from PeriodicWave");

        public:
        static constexpr auto signature = uti::concatenate(uti::concatenate(uti::to_array("inharmonicWave<"), Base::signature),uti::to_array(">"));
        WAVE_OPERATOR_OVERRIDE
        inharmonicWave() = default;
        inharmonicWave(const PhaseType increment) : Base(increment) {}

        void setup(const float ratios[harmonics], const float gains[harmonics])
        {
            for (size_t i = 0; i < harmonics; ++i)
            {
                m_ratios[i] = fpm::from_float<CoeffDescr>(ratios[i]);
                m_gains[i] = fpm::from_float<CoeffDescr>(gains[i]);
            }
        }

        inline SampleType getSample() const { return getSample(this->m_phase); }

        inline SampleType getSample(const PhaseType phase) const
        {
            SampleType sample = 0;
            for (size_t i = 0; i < harmonics; ++i)
            {
                sample += fpm::mul<SampleDescr,SampleDescr,CoeffDescr>(Base::getSample(fpm::mul<PhaseDescr,CoeffDescr,PhaseDescr>(m_ratios[i], (i+1) * phase)), m_gains[i]);
            }
            return sample;
        }

        CoeffType m_ratios[harmonics] = {0};
        CoeffType m_gains[harmonics] = {0};

        virtual int getParamCount() const  override { return 2*harmonics + Base::getParamCount(); }
        virtual SampleType* getParam(int index)  override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParam(index);
            }
            index -= baseCount;
            if (index < harmonics) return (SampleType *)&m_gains[index];
            index -= harmonics;
            if (index < harmonics) return (SampleType *)&m_ratios[index];
            return nullptr;
        }
        virtual const char *getParamName(int index) const override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParamName(index);
            }
            index -= baseCount;
            if (index < harmonics) return "inharmonicWave::Formant_Gain";	
            index -= harmonics;
            if (index < harmonics) return "inharmonicWave::Formant_Ratio";
            return nullptr;
        }
    };

    class envelope : public wave
    {
    public:
        static constexpr auto signature = uti::to_array("envelope");
        WAVE_OPERATOR_OVERRIDE
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

        inline void advance() { }
        inline SampleType getSample() const { return SampleMax; }
    };

    class envelopeBase : public envelope
    {
    public:
        static constexpr auto signature = uti::to_array("envelopeBase");
        WAVE_OPERATOR_OVERRIDE
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

        void setEnvRates(const SampleType attackRate, const SampleType decayRate, const SampleType releaseRate)
        {
            m_attackRate = attackRate;
            m_decayRate = decayRate;
            m_releaseRate = releaseRate;
        }

        void setSustainLevel(const SampleType sustainLevel)
        {
            m_sustainLevel = sustainLevel;
        }

        // time in seconds
        void setEnvTimes(const float attackTime, const float decayTime, const float releaseTime, const size_t sampleRate = 1)
        {
            m_attackRate = SampleMax / (attackTime * sampleRate);
            m_decayRate = SampleMax / (decayTime * sampleRate);
            m_releaseRate = SampleMax / (releaseTime * sampleRate);
        }

        inline void advance()
        {
            switch (m_state)
            {
            case State::attack:
                if (SampleMax - m_level < m_attackRate)
                {
                    m_level = SampleMax;
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

        SampleType getSample() const
        {
            return m_level;
        }

        State getEnvState() const override
        {
            return m_state;
        }

        SampleType m_attackRate = SampleMax;
        SampleType m_decayRate = SampleMax;
        SampleType m_sustainLevel = 0;
        SampleType m_releaseRate = SampleMax;
        SampleType m_level = 0;
        State m_state = State::idle;

        virtual int getParamCount() const  override { return 5; }
        virtual SampleType* getParam(int index)  override
        {
            if (index==0) return &m_attackRate;
            if (index==1) return &m_decayRate;
            if (index==2) return &m_sustainLevel;
            if (index==3) return &m_releaseRate;
            if (index==4) return &m_level;
            return nullptr;
        }
        virtual const char *getParamName(int index) const override
        {
            if (index==0) return "envelopeBase::AttackRate";
            if (index==1) return "envelopeBase::DecayRate";
            if (index==2) return "envelopeBase::SustainLevel";
            if (index==3) return "envelopeBase::ReleaseRate";
            if (index==4) return "envelopeBase::Level";
            return nullptr;
        }
    };

//next step envelope that takes the level from a buffer

    template <typename Base = sinWave>
    class gainModWave : public Base
    {
    public:
        static constexpr auto signature = uti::concatenate(uti::concatenate(uti::to_array("gainModWave<"), Base::signature),uti::to_array(">"));
        WAVE_OPERATOR_OVERRIDE

        gainModWave(const SampleType level = SampleMax) : m_level(level) {}

        void setLevel(const SampleType level) { m_level = level; }
        inline SampleType getLevel() { return m_level; }

        inline SampleType getSample() const
        {
            return fpm::mul<SampleDescr, SampleDescr, LevelDescr>(Base::getSample(), m_level);
        }
        inline void advance() { Base::advance(); }

        SampleType m_level = SampleMax;

        virtual int getParamCount() const  override { return 1 + Base::getParamCount(); }
        virtual SampleType* getParam(int index)  override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParam(index);
            }
            index -= baseCount;
            if (index==0) return &m_level;
            return nullptr;
        }
        virtual const char *getParamName(int index) const override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParamName(index);
            }
            index -= baseCount;
            if (index==0) return "gainModWave::Level";
            return nullptr;
        }
    };

    template <typename Carrier = sinWave, typename Modulator = envelopeBase>
    class amModWave : public wave
    {
    public:
        static constexpr auto signature = uti::concatenate(uti::concatenate(uti::to_array("amModWave<"), Carrier::signature),uti::concatenate(uti::to_array(", "),uti::concatenate(Modulator::signature,uti::to_array(">"))));
        WAVE_OPERATOR_OVERRIDE
        amModWave() = default;

        inline SampleType getSample() const
        {
            const SampleType level = m.getSample();
            return fpm::mul<SampleDescr, SampleDescr, LevelDescr>(c.getSample(), level);
        }
        inline void advance() { c.advance(); m.advance(); }

    public:
        Carrier c;
        Modulator m;

        virtual int getParamCount() const  override { return c.getParamCount() + m.getParamCount() + 2; }
        virtual SampleType* getParam(int index)  override
        {
            if (index==0) return nullptr; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int carrierCount = c.getParamCount();
            if (index<carrierCount)
            {
                return c.getParam(index);
            }
            index -= carrierCount;
            if (index==0) return nullptr; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int modulatorCount = m.getParamCount();
            if (index<modulatorCount)
            {
                return m.getParam(index);
            }
            return nullptr;
        }
        virtual const char *getParamName(int index) const override
        {
            if (index==0) return "amModWave::Carrier Parameters:"; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int carrierCount = c.getParamCount();
            if (index<carrierCount)
            {
                return c.getParamName(index);
            }
            index -= carrierCount;
            if (index==0) return "amModWave::Modulator Parameters:"; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int modulatorCount = m.getParamCount();
            if (index<modulatorCount)
            {
                return m.getParamName(index);
            }
            index -= modulatorCount;
            if (index==0) return "amModWave::End of parameters"; //for the "title" parameter
            return nullptr;
        }
    };

    template <typename Carrier = sinWave, typename Modulator = sinWave>
    class pmModWave : public wave
    {
        static_assert(std::is_base_of<periodicWave, Carrier>::value, "Carrier must be derived from periodicWave");
    public:
        static constexpr auto signature = uti::concatenate(uti::concatenate(uti::to_array("pmModWave<"), Carrier::signature),uti::concatenate(uti::to_array(", "),uti::concatenate(uti::to_array(Modulator::signature),uti::to_array(">"))));
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

        virtual int getParamCount() const  override { return c.getParamCount() + m.getParamCount() + 3; }
        virtual SampleType* getParam(int index)  override
        {
            if (index==0) return nullptr; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int carrierCount = c.getParamCount();
            if (index<carrierCount)
            {
                return c.getParam(index);
            }
            index -= carrierCount;
            if (index==0) return nullptr; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int modulatorCount = m.getParamCount();
            if (index<modulatorCount)
            {
                return m.getParam(index);
            }
            return nullptr;
        }
        virtual const char *getParamName(int index) const override
        {
            if (index==0) return "pmModWave::Carrier Parameters:"; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int carrierCount = c.getParamCount();
            if (index<carrierCount)
            {
                return c.getParamName(index);
            }
            index -= carrierCount;
            if (index==0) return "pmModWave::Modulator Parameters:"; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int modulatorCount = m.getParamCount();
            if (index<modulatorCount)
            {
                return m.getParamName(index);
            }
            index -= modulatorCount;
            if (index==0) return "pmModWave::End of parameters"; //for the "title" parameter
            return nullptr;
        }
    };

    template <typename Carrier = sinWave, typename Modulator = sinWave>
    class fmModWave : public wave
    {
    public:
        static constexpr auto signature = uti::concatenate(uti::concatenate(uti::to_array("fmModWave<"), Carrier::signature),uti::concatenate(uti::to_array(", "),uti::concatenate(uti::to_array(Modulator::signature),uti::to_array(">"))));
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

        virtual int getParamCount() const  override { return c.getParamCount() + m.getParamCount() + 3; }
        virtual SampleType* getParam(int index)  override
        {
            if (index==0) return nullptr; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int carrierCount = c.getParamCount();
            if (index<carrierCount)
            {
                return c.getParam(index);
            }
            index -= carrierCount;
            if (index==0) return nullptr; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int modulatorCount = m.getParamCount();
            if (index<modulatorCount)
            {
                return m.getParam(index);
            }
            return nullptr;
        }
        virtual const char *getParamName(int index) const override
        {
            if (index==0) return "fmModWave::Carrier Parameters:"; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int carrierCount = c.getParamCount();
            if (index<carrierCount)
            {
                return c.getParamName(index);
            }
            index -= carrierCount;
            if (index==0) return "fmModWave::Modulator Parameters:"; //for the "title" parameter
            index -= 1; //for the "title" parameter
            const int modulatorCount = m.getParamCount();
            if (index<modulatorCount)
            {
                return m.getParamName(index);
            }
            index -= modulatorCount;
            if (index==0) return "fmModWave::End of parameters"; //for the "title" parameter
            return nullptr;
        }
    };

    template <typename Base = sawtoothWave>
    class RBJFilterWave : public Base, public RBJ
    {
    public:
        static constexpr auto signature = uti::concatenate(uti::concatenate(uti::to_array("RBJFilterWave<"), Base::signature),uti::to_array(">"));
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
    
    //in order to define parameters, I would have to have the coefficients as a lookup based on SampleType parameters
    //for type of filter, cutoff/center frequency, Q, gain, etc.
    };
}