#pragma once
#include "fpm/fixed.hpp"
#include <limits>

using DefaultSampleType = fpm::fixed<16, 16>;
using DefaultAngleType = fpm::fixed<16, 16>;
using DefaultTimeType = fpm::fixed<16, 16>;
const size_t DefaultBufferSize = 1024;

template <typename SampleType, typename GainType>
SampleType applyGain(SampleType sample, GainType gain)
{
    return sample * gain;
}

template<size_t I1, size_t F1, bool S1, bool R1, size_t I2, size_t F2, bool S2, bool R2>
fpm::fixed<I1, F1, S1, R1> applyGain(const fpm::fixed<I1, F1, S1, R1> sample, const fpm::fixed<I2, F2, S2, R2> gain)
{
    if (S1)
    {
        using IntermediateType = typename fpm::detail::type_from_size<I1 + I2 + F1 + F2>::signed_type;
        return fpm::fixed<I1, F1, S1>::from_raw_value(((IntermediateType)sample.raw_value() * gain.raw_value()) >> (I2 + F2);
    }
    else
    {
        using IntermediateType = typename fpm::detail::type_from_size<I1 + I2 + F1 + F2>::unsigned_type;
        return fpm::fixed<I1, F1, S1>::from_raw_value(((IntermediateType)sample.raw_value() * gain.raw_value()) >> (I2 + F2));
    }
}

template <size_t I, size_t F, bool S, bool R, typename GainType>
fpm::fixed<I, F, S, R> applyGain(const fpm::fixed<I, F, S, R> sample, const GainType gain, typename ::std::enable_if<::std::is_integral<GainType>::value>::type* = nullptr)
{
    constexpr size_t I2 = 8*sizeof(GainType);
    if (S)
    {
        using IntermediateType = typename fpm::detail::type_from_size<I + F + I2>::signed_type;
        return fpm::fixed<I, F, S, R>::from_raw_value(((IntermediateType)sample.raw_value() * gain) >> I2);
    }
    else
    {
        using IntermediateType = typename fpm::detail::type_from_size<I + F + I2>::unsigned_type;
        return fpm::fixed<I, F, S, R>::from_raw_value(((IntermediateType)sample.raw_value() * gain) >> I2);
    }
}

template <typename SampleType = DefaultSampleType, typename AngleType = DefaultAngleType>
class wave
{
    public:
    wave() = default;

    SampleType getSample() const { return m_level; }
    void advance() { }
    void setPhase(const AngleType phase) { }
    void setLevel(const SampleType level) { m_level = level; }
    void getLevel() { return m_level; }

protected:
    SampleType m_level = numeric_limits<SampleType>::max();
};

template <typename SampleType = DefaultSampleType, typename srcType>
void fillBuffer(SampleType *buffer, size_t bufferSize, srcType src)
{
    for (size_t i = 0; i < bufferSize; ++i)
    {
        buffer[i] = src.getSample();
        src.advance();
    }
}

template <typename SampleType = DefaultSampleType, typename AngleType = DefaultAngleType>
class periodicWave : public wave<SampleType, AngleType>
{
    public:
    periodicWave() = default;

    void setFrequency(const AngleType frequency, const size_t sample_rate = 1) { m_increment = frequency / sample_rate; }
    void getFrequency(const size_t sample_rate = 1) { return m_increment * sample_rate; }
    void setPhase(const AngleType phase) { m_phase = phase; }
    void advance() { m_phase += m_increment; }
    void advance(const AngleType increment, const size_t sample_rate = 1) { m_phase += increment / sample_rate; }
    void getPhase() { return m_phase; }
    void getSample() const { return getSampleImpl(m_phase); }
    void getSample(const AngleType phase) const { return m_level; }

    protected:
    AngleType m_increment = AngleType(0);
    AngleType m_phase = AngleType(0);
};

template <typename SampleType = DefaultSampleType, typename AngleType = DefaultAngleType>
class squareWave : public periodicWave<SampleType, AngleType>
{
    public:
    squareWave() = default;

    SampleType getSample(const AngleType phase) const
    {
        return (int)(phase * 2) & 1 ? m_level : -m_level;
    }
};

template <typename T>
T fraction(T x)
{
    return x - (int)x;
}

template <size_t I, size_t F, bool S, bool R>
fpm::fixed<I, F, S, R> fraction(fpm::fixed<I, F, S, R> x)
{
    return x.fraction();
}

template <typename SampleType = DefaultSampleType, typename AngleType = DefaultAngleType>
class sawtoothWave : public periodicWave<SampleType, AngleType>
{
    public:
    sawtoothWave() = default;

    SampleType getSample(const AngleType phase) const
    {
        return m_level * fraction(phase - 0.5);
    }
};

constexpr int log2(const int n, const int p = 0) {
    return (n <= 1) ? p : log2(n >> 1, p + 1);
}

template <const void *buffer, size_t bufferSize = DefaultBufferSize, typename SampleType = DefaultSampleType, typename AngleType = DefaultAngleType>
class lookupWave : public periodicWave<SampleType, AngleType>
{
    static_assert((bufferSize > 0) && ((bufferSize & (bufferSize - 1)) == 0), "bufferSize must be a power of 2");

    constexpr static fpm::fixed<15, 1> fp_size = fpm::fixed<15, 1>(bufferSize);
    constexpr static int bufferSize_bits = log2(bufferSize);
    constexpr static int bufferSize_mask = (1 << buffersize_bits) - 1;

public:
    lookupWave() = default;

    SampleType getSample(const AngleType phase) const
    {
        const SampleType *sampleBuffer = static_cast<const SampleType *>(buffer);
        const SampleType s =  sampleBuffer[(size_t)((phase < 0 ? fp_size : 0) + (fp_size * phase)) & bufferSize_mask];
        return applyGain(s, m_level);

    }
};

template <size_t bufferSize = DefaultBufferSize, typename SampleType = DefaultSampleType, typename AngleType = DefaultAngleType>
class bufferWave : public periodicWave<SampleType, AngleType>
{
    static_assert((bufferSize > 0) && ((bufferSize & (bufferSize - 1)) == 0), "bufferSize must be a power of 2");

    constexpr static fpm::fixed<15, 1> fp_size = fpm::fixed<15, 1>(bufferSize);
    constexpr static int bufferSize_bits = log2(bufferSize);
    constexpr static int bufferSize_mask = (1 << buffersize_bits) - 1;

    public:
    bufferWave(const SampleType *buffer) : m_buffer(buffer) {}

    void setBuffer(const SampleType *buffer)
    {
        m_buffer = buffer;
    }

    SampleType getSample(const AngleType phase) const
    {
        const SampleType s = m_buffer[(size_t)((phase < 0 ? fp_size : 0) + (fp_size * phase)) & bufferSize_mask];
        return applyGain(s, m_level);
    }

private:
    const SampleType *m_buffer = nullptr;
};

//Maybe add interpolated lookup buffer and buffer wave classes

template <typename SampleType = DefaultSampleType, typename AngleType = DefaultAngleType>
class harmonicWave : periodicWave<SampleType, AngleType>
{
    public:
    harmonicWave() = default;
    harmonicWave(const periodicWave<SampleType, AngleType> *wave, const SampleType *harmonics, size_t numHarmonics)
        : m_wave(wave), m_harmonics(harmonics), m_numHarmonics(numHarmonics) {}

    void setWave(const periodicWave<SampleType, AngleType> *wave) { m_wave = wave; }

    void setHarmonics(const SampleType *harmonics, size_t numHarmonics)
    {
        m_harmonics = harmonics;
        m_numHarmonics = numHarmonics;
    }

    SampleType getSample(const AngleType phase) const
    {
        SampleType sample = 0;
        for (size_t i = 0; i < m_numHarmonics; ++i)
        {
            sample += applyGain(m_wave->getSample((i + 1) * phase), m_harmonics[i]);
        }
        return applyGain(sample, m_level);
    }

private:
    periodicWave<SampleType, AngleType> *m_wave = nullptr;
    const SampleType *m_harmonics = nullptr;
    size_t m_numHarmonics = 0;
};

template <typename SampleType = DefaultSampleType, typename AngleType = DefaultAngleType>
class inharmonicWave : periodicWave<SampleType, AngleType>
{
    public:
    inharmonicWave() = default;
    inharmonicWave(const periodicWave<SampleType, AngleType> *wave, const AngleType *ratios, size_t numRatios)
        : m_wave(wave), m_ratios(ratios), m_numRatios(numRatios) {}

    void setWave(const periodicWave<SampleType, AngleType> *wave) { m_wave = wave; }

    void setRatios(const AngleType *ratios, size_t numRatios)
    {
        m_ratios = ratios;
        m_numRatios = numRatios;
    }

    SampleType getSample(const AngleType phase) const
    {
        SampleType sample = 0;
        for (size_t i = 0; i < m_numRatios; ++i)
        {
            sample += m_wave->getSample(m_ratios[i] * phase);
        }
        return applyGain(sample, m_level);
    }

private:
    periodicWave<SampleType, AngleType> *m_wave = nullptr;
    const AngleType *m_ratios = nullptr;
    size_t m_numRatios = 0;
};

//TODO: add envelope generators and modulators

template <typename TimeType = DefaultTimeType>
class envelope
{
    //assert that TimeType is either floating point or fpm::fixed
    static_assert(::std::is_floating_point<TimeType>::value || ::std::is_same<TimeType, fpm::fixed<>>::value, "TimeType must be floating point or fpm::fixed");

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
    
    //time in seconds
    envelope(const TimeType attackRate, const TimeType decayRate, const TimeType sustainLevel, const TimeType releaseRate)
        : m_attackRate(attackRate), m_decayRate(decayRate), m_sustainLevel(sustainLevel), m_releaseRate(releaseRate) {}

    void attck() { m_state = State::attack; }
    void decay() { m_state = State::decay; }
    void sustain() { m_state = State::sustain; }
    void release() { m_state = State::release; }
    void stop() { m_state = State::idle; m_level = TimeType(0); }

    void setAttackRate(const TimeType attackRate) { m_attackRate = attackRate; }
    void setDecayRate(const TimeType decayRate) { m_decayRate = decayRate; }
    void setSustainLevel(const TimeType sustainLevel) { m_sustainLevel = sustainLevel; }
    void setReleaseRate(const TimeType releaseRate) { m_releaseRate = releaseRate; }

    TimeType getAttackRate() const { return m_attackRate; }
    TimeType getDecayRate() const { return m_decayRate; }
    TimeType getSustainLevel() const { return m_sustainLevel; }
    TimeType getReleaseRate() const { return m_releaseRate; }

    //time in seconds
    void setAttackTime(const TimeType attackTime, const size_t sampleRate = 1) { m_attackRate = TimeType(1) / (attackTime * sampleRate); }
    void setDecayTime(const TimeType decayTime, const size_t sampleRate = 1) { m_decayRate = TimeType(1) / (decayTime * sampleRate); }
    void setReleaseTime(const TimeType releaseTime, const size_t sampleRate = 1) { m_releaseRate = TimeType(1) / (releaseTime * sampleRate); }
    
    void advance()
    {
        switch (m_state)
        {
            case State::attack:
                m_level += m_attackRate;
                if (m_level >= TimeType(1))
                {
                    m_level = TimeType(1);
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
                if (m_level <= TimeType(0))
                {
                    m_level = TimeType(0);
                    m_state = State::idle;
                }
                break;
            default:
                break;
        }
    }

    //between 0 and 1
    TimeType getGain() const
    {
        return m_level;
    }

    State getState() const
    {
        return m_state;
    }

    void setGain(const TimeType level)
    {
        m_level = level;
    }

private:
    TimeType m_attackRate = TimeType(0);
    TimeType m_decayRate = TimeType(0);
    TimeType m_sustainLevel = TimeType(0);
    TimeType m_releaseRate = TimeType(0);
    TimeType m_level = TimeType(0);
    State m_state = State::idle;
};

template <typename SampleType = DefaultSampleType, typename AngleType, typename TimeType = DefaultTimeType>
class signalGenerator
{
    public:
    signalGenerator() = default;
    signalGenerator(const wave<SampleType, AngleType> *wave, const envelope<TimeType> *envelope) : m_wave(wave), m_envelope(envelope) {}

    void setWave(const wave<SampleType, AngleType> *wave) { m_wave = wave; }
    void setEnvelope(const envelope<TimeType> *envelope) { m_envelope = envelope; }

    void advance()
    {
        m_wave->advance();
        m_envelope->advance();
    }

    SampleType getSample() const
    {
        return applyGain(m_wave->getSample(), m_envelope->getGain());
    }

    void attack() { m_envelope->attack(); }
    void decay() { m_envelope->decay(); }
    void sustain() { m_envelope->sustain(); }
    void release() { m_envelope->release(); }
    void stop() { m_envelope->stop(); }

private:
    wave<SampleType, AngleType> *m_wave = nullptr;
    envelope<TimeType> *m_envelope = nullptr;
};