#pragma once

#include "wave_builtin.h"
#include "envelope.h"

namespace dsp
{
    template <typename Base = sinWave>
    class gainModWave : public Base
    {
    public:
        set_signature<uti::str("gainModWave"), sig_of(Base)>;
        variant_inherit(Base)
        WAVE_OPERATOR_OVERRIDE

        gainModWave(const SampleType level = SampleMax) : m_level(level) {}

        void setLevel(const SampleType level) { m_level = level; }
        inline SampleType getLevel() { return m_level; }

        inline SampleType getSample()
        {
            return fpm::mul<SampleDescr, SampleDescr, LevelDescr>(Base::getSample(), m_level);
        }

        inline SampleType getSample(const PhaseType phase) const
        {
            return fpm::mul<SampleDescr, SampleDescr, LevelDescr>(Base::getSample(phase), m_level);
        }

        inline void advance() { Base::advance(); }

        SampleType m_level = SampleMax;

        virtual int getParamCount() const  override { return 1 + Base::getParamCount(); }
        virtual SampleType* getParam(uint16_t index) override
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
        virtual const char *getParamName(uint16_t index) const override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParamName(index);
            }
            index -= baseCount;
            if (index==0) return "gainModWave::Level  [Min:Max] --> [-1:1] gain";
            return nullptr;
        }
    };

    template <typename Carrier = sinWave, typename Modulator = envelopeBase>
    class amModWave : public wave
    {
    public:
        set_signature<uti::str("amModWave"), sig_of(Carrier), sig_of(Modulator)>;
        variant_inherit(wave)
        WAVE_OPERATOR_OVERRIDE
        amModWave() = default;

        inline SampleType getSample()
        {
            const SampleType level = m.getSample();
            return fpm::mul<SampleDescr, SampleDescr, LevelDescr>(c.getSample(), level);
        }

        inline SampleType getSample(const PhaseType phase) const
        {
            const SampleType level = m.getSample();
            return fpm::mul<SampleDescr, SampleDescr, LevelDescr>(c.getSample(phase), level);
        }

        inline void advance() { c.advance(); m.advance(); }

    public:
        Carrier c;
        Modulator m;

        virtual int getParamCount() const  override { return c.getParamCount() + m.getParamCount() + 2; }
        virtual SampleType* getParam(uint16_t index) override
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
        virtual const char *getParamName(uint16_t index) const override
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
        set_signature<uti::str("pmModWave"), sig_of(Carrier), sig_of(Modulator)>;
        variant_inherit(wave)
        WAVE_OPERATOR_OVERRIDE
        pmModWave() = default;

        inline SampleType getSample()
        {
            SampleType level = m.getSample();
            SampleType s = c.getSample(level);
            return s;
        }

        inline SampleType getSample(const PhaseType phase) const
        {
            SampleType level = m.getSample();
            SampleType s = c.getSample(level + phase);
            return s;
        }

        inline void advance()
        {
            m.advance();
            c.advance();
        }

    public:
        Carrier c;
        Modulator m;

        virtual int getParamCount() const  override { return c.getParamCount() + m.getParamCount() + 3; }
        virtual SampleType* getParam(uint16_t index) override
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
        virtual const char *getParamName(uint16_t index) const override
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
        set_signature<uti::str("fmModWave"), sig_of(Carrier), sig_of(Modulator)>;
        variant_inherit(wave)
        WAVE_OPERATOR_OVERRIDE
        fmModWave() = default;

        inline SampleType getSample()
        {
            return c.getSample();
        }

        inline SampleType getSample(const PhaseType phase) const
        {
            return c.getSample(phase);
        }

        inline void advance()
        {
            SampleType level = m.getSample();
            m.advance();
            c.advance(level);
        }
    
    public:
        Carrier c;
        Modulator m;

        virtual int getParamCount() const  override { return c.getParamCount() + m.getParamCount() + 3; }
        virtual SampleType* getParam(uint16_t index) override
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
        virtual const char *getParamName(uint16_t index) const override
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
}