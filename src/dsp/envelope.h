#pragma once

#include "wave_root.h"

namespace dsp
{
    //next step envelope that takes the level from a buffer

    class envelope : public wave
    {
    public:
        set_signature<uti::str("envelope")>;
        variant_inherit(wave)
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

        virtual inline void attack() override { }
        virtual inline void decay() { }
        virtual inline void sustain() { }
        virtual inline void release() override { }
        virtual inline void stop() override { }
        virtual inline State getEnvState() const { return State::idle; }

        inline void advance() { }
        inline SampleType getSample() { return SampleMax; }
    };

    class envelopeBase : public envelope
    {
    public:
        set_signature<uti::str("envelopeBase")>;
        variant_inherit(envelope)
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

        SampleType getSample()
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
        virtual SampleType* getParam(uint16_t index) override
        {
            if (index==0) return &m_attackRate;
            if (index==1) return &m_decayRate;
            if (index==2) return &m_sustainLevel;
            if (index==3) return &m_releaseRate;
            if (index==4) return &m_level;
            return nullptr;
        }
        virtual const char *getParamName(uint16_t index) const override
        {
            if (index==0) return "envelopeBase::AttackRate 1/time [0:Max] --> [inf:0] time";
            if (index==1) return "envelopeBase::DecayRate 1/time [0:Max] --> [inf:0] time";
            if (index==2) return "envelopeBase::SustainLevel [0:Max] --> [0:SampleMax] target output";
            if (index==3) return "envelopeBase::ReleaseRate 1/time [0:Max] --> [inf:0] time";
            if (index==4) return "envelopeBase::Level [0:Max] --> [0:SampleMax] current output";
            return nullptr;
        }
    };
}