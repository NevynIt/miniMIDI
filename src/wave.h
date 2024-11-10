#pragma once
#include "hwConfig.h"

#define WAVEFORM_SIZE_BITS 10
#define WAVEFORM_SIZE (1 << WAVEFORM_SIZE_BITS)
#define WAVEFORM_SIZE_MASK (WAVEFORM_SIZE - 1)

typedef fpm::fixed<WAVEFORM_SIZE_BITS, 16 - WAVEFORM_SIZE_BITS, false> fp_index;
typedef fpm::fixed<16-7, 7, false> fp_ratio;

inline constexpr sample_t scale(sample_t x, fp_ratio r)
{
    return static_cast<sample_t>((int32_t)x * r.raw_value() / (1 << 7));
}

inline constexpr sample_t scale(sample_t x, sample_t r)
{
    return static_cast<sample_t>((int32_t)x * r / SAMPLE_MAX);
}

inline constexpr fp_index scale(fp_index x, fp_ratio r)
{
    return fp_index::from_raw_value(static_cast<int16_t>((int32_t)x.raw_value() * r.raw_value() / (1 << 7)));
}

inline constexpr sample_t mix(sample_t x, sample_t y, sample_t balance = 0) // balance i 0 for 50% mix, SAMPLE_MIN for 100% x, SAMPLE_MAX for 100% y
{
    return static_cast<sample_t>((((int32_t)balance + SAMPLE_MAX) * x + ((int32_t)SAMPLE_MAX - balance) * y) / SAMPLE_MAX);
}

// Wave class for generating (and modulating) waves from a look up table
class wave
{
public:
    wave(sample_cptr waveform) : waveform(waveform), index(0), increment(1) {}
    wave(sample_cptr waveform, fp_index frequency) : waveform(waveform), index(0), increment(0) { setFrequency(frequency); }
    wave(sample_cptr waveform, fp_index frequency, fp_index phase) : waveform(waveform), index(phase), increment(0) { setFrequency(frequency); }
    wave(sample_cptr waveform, float frequency) : waveform(waveform), index(0), increment(0) { setFrequency_f(frequency); }
    wave(sample_cptr waveform, float frequency, float phase) : waveform(waveform), index(0), increment(0)
    {
        setFrequency_f(frequency);
        setPhase_f(phase);
    }

    inline void setFrequency(fp_index frequency, uint32_t sample_rate = SAMPLE_RATE) { increment = frequency / sample_rate; }
    inline fp_index getFrequency(uint32_t sample_rate = SAMPLE_RATE) { return increment * sample_rate; }
    inline void setPhase(fp_index phase) { index = phase; }
    inline fp_index getPhase() { return index; }

    inline void setFrequency_f(float frequency, uint32_t sample_rate = SAMPLE_RATE) { increment = (fp_index)((frequency * WAVEFORM_SIZE) / sample_rate); }
    inline float getFrequency_f(uint32_t sample_rate = SAMPLE_RATE) { return ((float)increment * sample_rate) / WAVEFORM_SIZE; }
    inline void setPhase_f(float phase) { index = (fp_index)(phase * (1 << (16 - WAVEFORM_SIZE_BITS))); }
    inline float getPhase_f() { return (float)index / (1 << (16 - WAVEFORM_SIZE_BITS)); }

    inline void reset() { index = 0; }

    // no modulation
    inline sample_t getSample()
    {
        index += increment;
        return waveform[static_cast<uint16_t>(index.raw_value()) >> (16 - WAVEFORM_SIZE_BITS)];
    }
    // frequency (or phase) modulation
    inline sample_t getSample_F(fp_index mod_FM)
    {
        index += increment + mod_FM;
        return waveform[static_cast<uint16_t>(index.raw_value()) >> (16 - WAVEFORM_SIZE_BITS)];
    }
    inline sample_t getSample_F(fp_ratio mod_FM)
    {
        index += increment + scale(increment, mod_FM);
        return waveform[static_cast<uint16_t>(index.raw_value()) >> (16 - WAVEFORM_SIZE_BITS)];
    }
    // amplitude (or ring) modulation
    inline sample_t getSample_A(fp_ratio mod_AM)
    {
        index += increment;
        return scale(waveform[static_cast<uint16_t>(index.raw_value()) >> (16 - WAVEFORM_SIZE_BITS)], mod_AM);
    }
    inline sample_t getSample_A(sample_t mod_AM)
    {
        index += increment;
        return scale(waveform[static_cast<uint16_t>(index.raw_value()) >> (16 - WAVEFORM_SIZE_BITS)], mod_AM);
    }
    // amplitude and frequency modulation
    inline sample_t getSample_AF(fp_ratio mod_AM, fp_index mod_FM)
    {
        index += increment + mod_FM;
        return scale(waveform[static_cast<uint16_t>(index.raw_value()) >> (16 - WAVEFORM_SIZE_BITS)], mod_AM);
    }
    inline sample_t getSample_AF(fp_ratio mod_AM, fp_ratio mod_FM)
    {
        index += increment + scale(increment, mod_FM);
        return scale(waveform[static_cast<uint16_t>(index.raw_value()) >> (16 - WAVEFORM_SIZE_BITS)], mod_AM);
    }

    sample_cptr waveform;
    fp_index index;     // equivalent of phase, with WAVEFORM_SIZE_BITS bits for the integer part and (16 - WAVEFORM_SIZE_BITS) bits for the fractional part
    fp_index increment; // equivalent of frequency, scaled by WAVEFORM_SIZE / SAMPLE_RATE
};

class wave_const
{
public:
    wave_const(sample_t lvl) : level(lvl) {}

    inline void setFrequency(fp_index frequency) { return; }
    inline fp_index getFrequency() { return fp_index(1); }
    inline void setPhase(fp_index phase) { return; }
    inline fp_index getPhase() { return fp_index(0); }

    inline void setFrequency_f(float frequency) { return; }
    inline float getFrequency_f() { return 1.0f; }
    inline void setPhase_f(float phase) { return; }
    inline float getPhase_f() { return 0.0f; }

    // no modulation
    inline sample_t getSample()
    {
        return level;
    }
    // frequency (or phase) modulation
    inline sample_t getSample_F(fp_index mod_FM)
    {
        return level;
    }
    inline sample_t getSample_F(fp_ratio mod_FM)
    {
        return level;
    }
    // amplitude (or ring) modulation
    inline sample_t getSample_A(fp_ratio mod_AM)
    {
        return scale(level, mod_AM);
    }
    inline sample_t getSample_A(sample_t mod_AM)
    {
        return scale(level, mod_AM);
    }
    // amplitude and frequency modulation
    inline sample_t getSample_AF(fp_ratio mod_AM, fp_index mod_FM)
    {
        return scale(level, mod_AM);
    }
    inline sample_t getSample_AF(fp_ratio mod_AM, fp_ratio mod_FM)
    {
        return scale(level, mod_AM);
    }

    sample_t level;
};

template<class C, class M>
class wave_mod_FM
{
    public:
    wave_mod_FM(C &carrier_wave, M &modulator_wave, float freq_mod_range) : carrier(carrier_wave), modulator(modulator_wave) { strength = (fp_ratio)((freq_mod_range * WAVEFORM_SIZE) / SAMPLE_MAX); } 

    inline void setFrequency(fp_index carrier_freq) { auto ratio = getRatio(); carrier.setFrequency(carrier_freq); modulator.setFrequency(scale(carrier_freq, ratio)); }
    inline fp_index getFrequency() { return carrier.getFrequency(); }

    inline void setCFrequency(fp_index carrier_freq) { carrier.setFrequency(carrier_freq); }
    inline fp_index getCFrequency() { return carrier.getFrequency(); }
    inline void setMFrequency(fp_index modulator_freq) { modulator.setFrequency(modulator_freq); }
    inline fp_index getMFrequency() { return modulator.getFrequency(); }

    inline void setCPhase(fp_index carrier_phase) { carrier.setPhase(carrier_phase); }
    inline fp_index getCPhase() { return carrier.getPhase(); }
    inline void setMPhase(fp_index modulator_phase) { modulator.setPhase(modulator_phase); }
    inline fp_index getMPhase() { return modulator.getPhase(); }

    inline void setStrength(float freq_mod_range) { strength = (fp_ratio)((freq_mod_range * WAVEFORM_SIZE) / SAMPLE_MAX); }
    inline float getStrength() { return ((float)strength * SAMPLE_MAX) / WAVEFORM_SIZE; }

    inline void setRatio(fp_ratio r) { modulator.setFrequency(scale(carrier.getFrequency(), r)); }
    inline fp_ratio getRatio() { return fp_ratio(modulator.getFrequency() / carrier.getFrequency()); }

    // no modulation of the carrier
    inline sample_t getSample()
    {
        fp_ratio fm = fp_ratio::from_raw_value(scale(modulator.getSample(), strength));
        return carrier.getSample_F(fm);
    }
    // frequency modulation of the carrier
    inline sample_t getSample_F(fp_index mod_FM)
    {
        fp_ratio fm = fp_ratio::from_raw_value(scale(modulator.getSample_F(mod_FM), strength));
        return carrier.getSample_F(fm);
    }
    inline sample_t getSample_F(fp_ratio mod_FM)
    {
        fp_ratio fm = fp_ratio::from_raw_value(scale(modulator.getSample_F(mod_FM), strength));
        return carrier.getSample_F(fm);
    }
    // amplitude modulation of the carrier
    inline sample_t getSample_A(fp_ratio mod_AM)
    {
        fp_ratio fm = fp_ratio::from_raw_value(scale(modulator.getSample_A(mod_AM), strength));
        return carrier.getSample_F(fm);
    }
    inline sample_t getSample_A(sample_t mod_AM)
    {
        fp_ratio fm = fp_ratio::from_raw_value(scale(modulator.getSample_A(mod_AM), strength));
        return carrier.getSample_F(fm);
    }
    // amplitude and frequency modulation of the carrier
    inline sample_t getSample_AF(fp_ratio mod_AM, fp_index mod_FM)
    {
        fp_ratio fm = fp_ratio::from_raw_value(scale(modulator.getSample_AF(mod_AM, mod_FM)));
        return carrier.getSample_F(fm);
    }
    inline sample_t getSample_AF(fp_ratio mod_AM, fp_ratio mod_FM)
    {
        fp_ratio fm = fp_ratio::from_raw_value(scale(modulator.getSample_AF(mod_AM, mod_FM)));
        return carrier.getSample_F(fm);
    }

    C &carrier;
    M &modulator;
    fp_ratio strength;
};

template<class C, class M>
class wave_mod_AM
{
    //Strengh calculation must be fixed like in wave_mod_FM
    public:
    wave_mod_AM(C &carrier_wave, M &modulator_wave, fp_ratio preamp = fp_ratio(1.0f)) : carrier(carrier_wave), modulator(modulator_wave), strength(preamp) {}
    inline void setFrequency(fp_index carrier_freq) { carrier.setFrequency(carrier_freq); modulator.setFrequency(carrier_freq); }
    inline fp_index getFrequency() { return carrier.getFrequency(); }

    inline void setCFrequency(fp_index carrier_freq) { carrier.setFrequency(carrier_freq); }
    inline fp_index getCFrequency() { return carrier.getFrequency(); }
    inline void setMFrequency(fp_index modulator_freq) { modulator.setFrequency(modulator_freq); }
    inline fp_index getMFrequency() { return modulator.getFrequency(); }
    
    inline void setCPhase(fp_index carrier_phase) { carrier.setPhase(carrier_phase); }
    inline fp_index getCPhase() { return carrier.getPhase(); }
    inline void setMPhase(fp_index modulator_phase) { modulator.setPhase(modulator_phase); }
    inline fp_index getMPhase() { return modulator.getPhase(); }

    inline void setRatio(fp_ratio r) { modulator.setFrequency(scale(carrier.getFrequency(), r)); }
    inline fp_ratio getRatio() { return fp_ratio(modulator.getFrequency() / carrier.getFrequency()); }

    // no modulation of the carrier
    inline sample_t getSample()
    {
        auto am = scale(modulator.getSample(), strength);
        return carrier.getSample_A(am);
    }
    // frequency modulation of the carrier
    inline sample_t getSample_F(fp_index mod_FM)
    {
        auto am = scale(modulator.getSample_F(mod_FM), strength);
        return carrier.getSample_A(am);
    }
    inline sample_t getSample_F(fp_ratio mod_FM)
    {
        auto am = scale(modulator.getSample_F(mod_FM), strength);
        return carrier.getSample_A(am);
    }
    // amplitude modulation of the carrier
    inline sample_t getSample_A(fp_ratio mod_AM)
    {
        auto am = scale(modulator.getSample_A(mod_AM), strength);
        return carrier.getSample_A(am);
    }
    inline sample_t getSample_A(sample_t mod_AM)
    {
        auto am = scale(modulator.getSample_A(mod_AM), strength);
        return carrier.getSample_A(am);
    }
    // amplitude and frequency modulation of the carrier
    inline sample_t getSample_AF(fp_ratio mod_AM, fp_index mod_FM)
    {
        auto am = scale(modulator.getSample_AF(mod_AM, mod_FM));
        return carrier.getSample_A(am);
    }
    inline sample_t getSample_AF(fp_ratio mod_AM, fp_ratio mod_FM)
    {
        auto am = scale(modulator.getSample_AF(mod_AM, mod_FM));
        return carrier.getSample_A(am);
    }

    C &carrier;
    M &modulator;
    fp_ratio strength;
};

template<class T>
void fill(sample_ptr buf, uint16_t size, T &wave)
{
    for (int i = 0; i < size; ++i)
        buf[i] = wave.getSample();
}

// // Wave class for simultaneous AM and FM modulation using one carrier and two modulators
// template<class C, class M1, class M2>
// class wave_mod_AMFM
// {
// public:
//     //Strengh calculation must be fixed like in wave_mod_FM
//     wave_mod_AMFM(C carrier_wave, M1 modulator_AM, M2 modulator_FM, fp_ratio strength_AM, fp_ratio strength_FM)
//         : carrier(carrier_wave), modulator_am(modulator_AM), modulator_fm(modulator_FM),
//           strength_am(strength_AM), strength_fm(strength_FM) {}

//     inline void setFrequency(fp_index carrier_freq) {
//         carrier.setFrequency(carrier_freq);
//         modulator_am.setFrequency(scale(carrier_freq, getAMRatio()));
//         modulator_fm.setFrequency(scale(carrier_freq, getFMRatio()));
//     }

//     inline fp_index getFrequency() { return carrier.getFrequency(); }

//     inline void setCPhase(fp_index carrier_phase) { carrier.setPhase(carrier_phase); }
//     inline fp_index getCPhase() { return carrier.getPhase(); }

//     inline void setAMRatio(fp_ratio r) { modulator_am.setFrequency(scale(carrier.getFrequency(), r)); }
//     inline fp_ratio getAMRatio() { return fp_ratio(modulator_am.getFrequency() / carrier.getFrequency()); }

//     inline void setFMRatio(fp_ratio r) { modulator_fm.setFrequency(scale(carrier.getFrequency(), r)); }
//     inline fp_ratio getFMRatio() { return fp_ratio(modulator_fm.getFrequency() / carrier.getFrequency()); }

//     inline void setStrengthAM(fp_ratio s) { strength_am = s; }
//     inline fp_ratio getStrengthAM() { return strength_am; }

//     inline void setStrengthFM(fp_ratio s) { strength_fm = s; }
//     inline fp_ratio getStrengthFM() { return strength_fm; }

//     inline sample_t getSample()
//     {
//         auto am = scale(modulator_am.getSample(), strength_am);
//         auto fm = scale(modulator_fm.getSample(), strength_fm);
//         return carrier.getSample_AMFM(am, fm);
//     }

// private:
//     C carrier;
//     M1 modulator_am;
//     M2 modulator_fm;
//     fp_ratio strength_am;
//     fp_ratio strength_fm;
// };

// ADSR class to generate envelopes (values between 0 and SAMPLE_MAX)
class ADSR
{
public:
    ADSR() : attack(0), sustain(0), decay(0), release(0), state(0), level(0) {}
    ADSR(sample_t a, sample_t d, sample_t s, sample_t r) : attack(a), decay(d), sustain(s), release(r), state(0), level(0) {}
    ADSR(float a, float d, float s, float r) : attack((sample_t)(a * (1 << 16))), decay((sample_t)(d * (1 << 16))), sustain((sample_t)(s * (1 << 16))), release((sample_t)(r * (1 << 16))), state(0), level(0) {}

    inline void setAttack(sample_t a) { attack = a; }
    inline sample_t getAttack() { return attack; }
    inline void setSustain(sample_t s) { sustain = s; }
    inline sample_t getSustain() { return sustain; }
    inline void setDecay(sample_t d) { decay = d; }
    inline sample_t getDecay() { return decay; }
    inline void setRelease(sample_t r) { release = r; }
    inline sample_t getRelease() { return release; }

    // Set parameter using time (must scale by SAMPLE_RATE)
    inline void setAttack_ms(fp_index a) { attack = (a * (SAMPLE_RATE / 1000)).integral(); }
    inline fp_index getAttack_ms() { return (fp_index)(attack) / (SAMPLE_RATE / 1000); }
    inline void setSustain_ms(fp_index s) { sustain = (s * (SAMPLE_RATE / 1000)).integral(); }
    inline fp_index getSustain_ms() { return (fp_index)(sustain) / (SAMPLE_RATE / 1000); }
    inline void setDecay_ms(fp_index d) { decay = (d * (SAMPLE_RATE / 1000)).integral(); }
    inline fp_index getDecay_ms() { return (fp_index)(decay) / (SAMPLE_RATE / 1000); }
    inline void setRelease_ms(fp_index r) { release = (r * (SAMPLE_RATE / 1000)).integral(); }
    inline fp_index getRelease_ms() { return (fp_index)(release); }

    // set parameter using float (must scale by (1 << 16))
    inline void setAttack_f(float a) { attack = (sample_t)(a * (1 << 16)); }
    inline float getAttack_f() { return (float)attack / (1 << 16); }
    inline void setSustain_f(float s) { sustain = (sample_t)(s * (1 << 16)); }
    inline float getSustain_f() { return (float)sustain / (1 << 16); }
    inline void setDecay_f(float d) { decay = (sample_t)(d * (1 << 16)); }
    inline float getDecay_f() { return (float)decay / (1 << 16); }
    inline void setRelease_f(float r) { release = (sample_t)(r * (1 << 16)); }
    inline float getRelease_f() { return (float)release / (1 << 16); }

    inline void noteAttack()
    {
        state = 1;
        level = 0;
    }

    inline void noteRelease()
    {
        state = 4;
    }

    inline void noteOff()
    {
        level = 0;
        state = 0;
    }

    inline sample_t getSample()
    {
        switch (state)
        {
        // case 0: //off
        //     level = 0;
        //     break;
        case 1: // attack
            if (level < SAMPLE_MAX)
            {
                level += attack;
            }
            else
            {
                level = SAMPLE_MAX;
                state = 2;
            }
            break;
        case 2: // decay
            if (level > sustain)
            {
                level -= decay;
            }
            else
            {
                level = sustain;
                state = 3;
            }
            break;
        // case 3: //sustain
        //     level = sustain;
        //     break;
        case 4: // release
            if (level > 0)
            {
                level -= release;
            }
            else
            {
                level = 0;
                state = 0;
            }
            break;
        }
        return level;
    }

    sample_t attack;
    sample_t sustain;
    sample_t decay;
    sample_t release;
    uint8_t state;
    sample_t level;
};