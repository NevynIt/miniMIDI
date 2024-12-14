#pragma once
#include "wave_root.h"

namespace dsp
{
    class constantWave : public wave
    {
    public:
        set_signature<uti::str("constantWave")>;
        variant_inherit(wave)
        WAVE_OPERATOR_OVERRIDE

        constantWave(const SampleType level = SampleMax) : m_level(level) {}

        inline SampleType getSample() { return m_level; }
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
        virtual const char *getParamName(uint16_t index) const
        {
            if (index==0) return "constantWave::Level [Min:Max] --> [SampleMin:SampleMax] fixed output";
            return nullptr;
        }
    };

    // #define FNC_WAVE(_name_, _fnc_, _N_params_, _signature_) \
    // class _name_##Wave : public wave \
    // { \
    // public: \
    //     static constexpr auto signature = uti::STR2A(_signature_); \
    //     static constexpr auto split_signature = uti::split(signature); \
    //     WAVE_OPERATOR_OVERRIDE \
    //     _name_##Wave() {} \
    //     \
    //     inline SampleType getSample() { return _fnc_(state); } \
    //     inline void advance() {} \
    //     \
    //     int getParamCount() const override { return _N_params_; } \
    //     SampleType* getParam(uint16_t index) override { \
    //         if (index < _N_params_) \
    //             return state + index; \
    //         return nullptr; \
    //     } \
    //     const char* getParamName(uint16_t index) const override { \
    //         if (index < _N_params_) \
    //         { \
    //             return uti::exectractParamName(index, split_signature); \
    //         } \
    //         return nullptr; \
    //     } \
    //     \
    //     SampleType state[_N_params_] = {0}; \
    // };

    // template<uint8_t N>
    // inline SampleType movingAverage(SampleType params[N])
    // {
    //     SampleType acc = 0;
    //     for (int i=N-1; i>=0; i--)
    //     {
    //         acc += params[i] / N;
    //         if (i<N-1)
    //             params[i] = params[i+1];
    //     }
    //     return acc;
    // }

    // //Example of fncWave
    // FNC_WAVE(movAvg, movingAverage<5>, 5, "movingAverage(input, x[-1], x[-2], x[-3], x[-4]) -> output")

    class noiseWave : public wave
    {
    public:
        set_signature<uti::str("noiseWave")>;
        variant_inherit(wave)
        WAVE_OPERATOR_OVERRIDE
        noiseWave() = default;

        inline SampleType getSample() { return rand()*2; }
        inline void advance() {}
    };

    class periodicWave : public wave
    {
    public:
        set_signature<uti::str("periodicWave")>;
        variant_inherit(wave)
        WAVE_OPERATOR_OVERRIDE
        periodicWave() = default;
        periodicWave(const PhaseType increment) : m_increment(increment) {}

        void setIncrement(const PhaseType increment) { m_increment = increment; }
        PhaseType getIncrement() { return m_increment; }        
        void setPhase(const PhaseType phase) { m_phase = phase; }
        inline void advance() { m_phase += m_increment; }
        inline void advance(const PhaseType increment) { m_phase += m_increment + increment; }
        PhaseType getPhase() { return m_phase; }
        inline SampleType getSample() { return getSample(m_phase); }
        inline SampleType getSample(const PhaseType phase) const { return 0; }

        PhaseType m_increment = 0;
        PhaseType m_phase = 0;

        virtual int getParamCount() const  override { return 2; }
        virtual SampleType* getParam(uint16_t index) override
        {
            if (index==0) return (SampleType *)&m_increment;
            if (index==1) return (SampleType *)&m_phase;
            return nullptr;
        }
        virtual const char *getParamName(uint16_t index) const override
        {
            if (index==0) return "periodicWave::Increment [0:unsigned Max] --> [0:1[ normalized frequency";
            if (index==1) return "periodicWave::Phase [Min:Max] --> [-pi:pi[ current radians";
            return nullptr;
        }
    };

    class squareWave : public periodicWave
    {
    public:
        set_signature<uti::str("squareWave")>;
        variant_inherit(periodicWave)
        WAVE_OPERATOR_OVERRIDE
        squareWave() = default;
        squareWave(const PhaseType increment) : periodicWave(increment) {}

        inline SampleType getSample()
        {
            return (m_phase < duty) ? SampleMax : SampleMin;
        }
        inline SampleType getSample(const PhaseType phase) const
        {
            return ((m_phase + phase) < duty) ? SampleMax : SampleMin;
        }

        PhaseType duty = fpm::from_float<SampleDescr>(0.5f);
        virtual int getParamCount() const  override { return 2; }
        virtual SampleType* getParam(uint16_t index) override
        {
            const int baseCount = periodicWave::getParamCount();
            if (index < baseCount)
                return periodicWave::getParam(index);
            index-=baseCount;
            if (index==0) return (SampleType *)&duty;
            return nullptr;
        }
        virtual const char *getParamName(uint16_t index) const override
        {
            const int baseCount = periodicWave::getParamCount();
            if (index < baseCount)
                return periodicWave::getParamName(index);
            index-=baseCount;
            if (index==0) return "squareWave::DutyCycle [0:unsigned Max] --> [0:1[";
            return nullptr;
        }
    };

    class sawtoothWave : public periodicWave
    {
    public:
        set_signature<uti::str("sawtoothWave")>;
        variant_inherit(periodicWave)
        WAVE_OPERATOR_OVERRIDE
        sawtoothWave() = default;
        sawtoothWave(const PhaseType increment) : periodicWave(increment) {}

        inline SampleType getSample()
        {
            return m_phase;
        }
        inline SampleType getSample(const PhaseType phase) const
        {
            return phase + m_phase;
        }
    };

    template <int count = BufferSize>
    class bufferWave : public periodicWave
    {
        static_assert((count > 1) && ((count & (count - 1)) == 0), "count must be a power of 2");
        constexpr static int count_bits = log2(count);
    public:
        set_signature<uti::str("bufferWave"), count>;
        variant_inherit(periodicWave)
        WAVE_OPERATOR_OVERRIDE
        bufferWave(const SampleType *buffer) : m_buffer(buffer) {}
        bufferWave(const SampleType *buffer, const PhaseType increment) : m_buffer(buffer) , periodicWave(increment) {}

        void setBuffer(const SampleType *buffer)
        {
            m_buffer = buffer;
        }

        inline SampleType getSample()
        {
            return m_buffer[upperBits<uint16_t, PhaseType, count_bits>(m_phase)];
        }

        inline SampleType getSample(const PhaseType phase) const
        {
            return m_buffer[upperBits<uint16_t, PhaseType, count_bits>(m_phase + phase)];
        }

        const SampleType *m_buffer = nullptr;
    };

    class builtinWave : public bufferWave<tables::DSP_SIZE>
    {
    public:
        set_signature<uti::str("builtinWave")>;
        variant_inherit(bufferWave<tables::DSP_SIZE>)
        WAVE_OPERATOR_OVERRIDE
        builtinWave() : bufferWave(tables::sinWave) {}
        builtinWave(const PhaseType increment) : bufferWave(tables::sinWave, increment) {}
        void select (const size_t i) { if (i<tables::DSP_COUNT) bufferWave::setBuffer(tables::DSP[i]); }
        void selectSin() { bufferWave::setBuffer(tables::sinWave); }
        void selectTriangle() { bufferWave::setBuffer(tables::triangleWave); }
        void selectSawtooth() { bufferWave::setBuffer(tables::sawtoothWave); }
        void selectSquare() { bufferWave::setBuffer(tables::squareWave); }
    };

    class sinWave : public builtinWave
    {
    public:
        set_signature<uti::str("sinWave")>;
        variant_inherit(builtinWave)
        WAVE_OPERATOR_OVERRIDE
        sinWave() : builtinWave() {}
        sinWave(const PhaseType increment) : builtinWave(increment) { selectSin(); }
    };

    template <size_t harmonics = 5, typename Base = sinWave>
    class harmonicWave : public Base
    {
        static_assert(std::is_base_of<periodicWave, Base>::value, "Base must be derived from periodicWave");

    public:
        set_signature<uti::str("harmonicWave"), harmonics, sig_of(Base)>;
        variant_inherit(Base)
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

        inline SampleType getSample()
        {
            SampleType sample = 0;
            for (size_t i = 0; i < harmonics; ++i)
            {
                sample += fpm::mul<SampleDescr,SampleDescr,CoeffDescr>(Base::getSample((i+1) * this->m_phase), m_harmonics[i]);
            }
            return sample;
        }

        inline SampleType getSample(const PhaseType phase) const
        {
            const PhaseType p = phase + this->m_phase;
            SampleType sample = 0;
            for (size_t i = 0; i < harmonics; ++i)
            {
                sample += fpm::mul<SampleDescr,SampleDescr,CoeffDescr>(Base::getSample((i+1) * p), m_harmonics[i]);
            }
            return sample;
        }

        CoeffType m_harmonics[harmonics] = {0};

        virtual int getParamCount() const  override { return harmonics + Base::getParamCount(); }
        virtual SampleType* getParam(uint16_t index) override
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
        virtual const char *getParamName(uint16_t index) const override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParamName(index);
            }
            index -= baseCount;
            if (index < harmonics) return "harmonicWave::Formant_Gain [Min:Max] --> [-2^4:2^4[ gain";
            return nullptr;
        }
    };

    template <size_t harmonics = 5, typename Base = sinWave>
    class inharmonicWave : public Base
    {
        static_assert(std::is_base_of<periodicWave, Base>::value, "Base must be derived from PeriodicWave");

        public:
        set_signature<uti::str("inharmonicWave"), harmonics, sig_of(Base)>;
        variant_inherit(Base)
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

        inline SampleType getSample()
        {
            SampleType sample = 0;
            for (size_t i = 0; i < harmonics; ++i)
            {
                sample += fpm::mul<SampleDescr,SampleDescr,CoeffDescr>(Base::getSample(fpm::mul<PhaseDescr,CoeffDescr,PhaseDescr>(m_ratios[i], (i+1) * this->m_phase)), m_gains[i]);
            }
            return sample;
        }

        inline SampleType getSample(const PhaseType phase) const
        {
            const PhaseType p = phase + this->m_phase;
            SampleType sample = 0;
            for (size_t i = 0; i < harmonics; ++i)
            {
                sample += fpm::mul<SampleDescr,SampleDescr,CoeffDescr>(Base::getSample(fpm::mul<PhaseDescr,CoeffDescr,PhaseDescr>(m_ratios[i], (i+1) * p)), m_gains[i]);
            }
            return sample;
        }

        CoeffType m_ratios[harmonics] = {0};
        CoeffType m_gains[harmonics] = {0};

        virtual int getParamCount() const  override { return 2*harmonics + Base::getParamCount(); }
        virtual SampleType* getParam(uint16_t index) override
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
        virtual const char *getParamName(uint16_t index) const override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParamName(index);
            }
            index -= baseCount;
            if (index < harmonics) return "inharmonicWave::Formant_Gain [Min:Max] --> [-2^4:2^4[ gain";	
            index -= harmonics;
            if (index < harmonics) return "inharmonicWave::Formant_Ratio [Min:Max] --> [-2^4:2^4[ freq ratio";
            return nullptr;
        }
    };
}