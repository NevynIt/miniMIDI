#include "wave_script_wave.h"

namespace dsp
{
#define DECL_LOGIC_WAVE(_name_) \
    class _name_##_wave : public logic_wave<_name_##_logic> \
    { \
    public: \
        _name_##_wave() = default; \
    };

#define DECL_SCRIPTABLE_WAVE(_name_) \
    class _name_##_wave_s : public script_wave<_name_##_logic> \
    { \
    public: \
        _name_##_wave_s() = default; \
    };

#define DECL_WAVE(_name_) \
    DECL_LOGIC_WAVE(_name_) \
    DECL_SCRIPTABLE_WAVE(_name_)

#define DECL_LOGIC_WAVE_T(_name_, _templ_param_) \
    class _name_##_wave : public logic_wave<_name_##_logic<_templ_param_>> \
    { \
    public: \
        _name_##_wave() = default; \
    };

#define DECL_SCRIPTABLE_WAVE_T(_name_, _templ_param_) \
    class _name_##_wave_s : public script_wave<_name_##_logic<_templ_param_>> \
    { \
    public: \
        _name_##_wave_s() = default; \
    };

#define DECL_WAVE_T(_name_, _templ_param_) \
    DECL_LOGIC_WAVE_T(_name_, _templ_param_) \
    DECL_SCRIPTABLE_WAVE_T(_name_, _templ_param_)

    class noise_logic : public logic_base<op0>
    {
    public:
        static constexpr auto signature = uti::STR2A("Noise");
        inline SampleType postSampling()
        {
            return (SampleType)rand();
        }
    };

    DECL_WAVE(noise)

    class constant_logic : public logic_base<op0>
    {
    public:
        static constexpr auto signature = uti::STR2A("Constant");
        inline SampleType postSampling()
        {
            return S.level;
        }

        struct // scratchpad  // metaprogramming virtual - "overload" in implementation
        { // only SampleType variables here
            SampleType level = SampleMax;
        } S;
        static constexpr const char *S_names[] = { "Level [Min:Max] --> [SampleMin:SampleMax] fixed output" }; // scratchpad parameter names
    };

    DECL_WAVE(constant)

    class square_logic : public logic_base<op0>
    {
    public:
        static constexpr auto signature = uti::STR2A("Square");
        inline SampleType postSampling()
        {
            return (S.phase < S.duty) ? SampleMax : SampleMin;
        }

        inline void postAdvance() // metaprogramming virtual function - "overload" in implementation
        {
            S.phase += S.increment;
        }

        struct // scratchpad  // metaprogramming virtual - "overload" in implementation
        { // only SampleType variables here
            PhaseType increment = 0;
            PhaseType phase = 0;
            PhaseType duty = fpm::from_float<SampleDescr>(0.5f);
        } S;

        static constexpr const char *S_names[] = { 
            "Increment [0:unsigned Max] --> [0:1[ normalized frequency",
            "Phase [Min:Max] --> [-pi:pi[ current radians",
            "DutyCycle [0:unsigned Max] --> [0:1["
        }; // scratchpad parameter names
    };

    DECL_WAVE(square)

    class sawtooth_logic : public logic_base<op0>
    {
    public:
        static constexpr auto signature = uti::STR2A("Sawtooth");
        inline SampleType postSampling()
        {
            return S.phase;
        }

        inline void postAdvance() // metaprogramming virtual function - "overload" in implementation
        {
            S.phase += S.increment;
            if (S.phase >= SampleMax)
                S.phase -= SampleMax;
        }

        struct // scratchpad  // metaprogramming virtual - "overload" in implementation
        { // only SampleType variables here
            PhaseType increment = 0;
            PhaseType phase = 0;
        } S;

        static constexpr const char *S_names[] = { 
            "Increment [0:unsigned Max] --> [0:1[ normalized frequency",
            "Phase [Min:Max] --> [SampleMin:SampleMax[ current sample"
        }; // scratchpad parameter names
    };

    DECL_WAVE(sawtooth)

    class buffer_logic : public logic_base<op0>
    {
    public:
        static constexpr auto signature = uti::STR2A("Buffer");
        inline SampleType postSampling()
        {
            return I.buffer[upperBits<uint16_t, PhaseType, BufferBits>(S.phase)];
        }

        inline void postAdvance() // metaprogramming virtual function - "overload" in implementation
        {
            S.phase += S.increment;
        }

        struct // scratchpad  // metaprogramming virtual - "overload" in implementation
        { // only SampleType variables here
            PhaseType increment = 0;
            PhaseType phase = 0;
        } S;
        static constexpr const char *S_names[] = { 
            "Increment [0:unsigned Max] --> [0:1[ normalized frequency",
            "Phase [Min:Max] --> [SampleMin:SampleMax[ current sample",
        }; // scratchpad parameter names

        struct // indirect // metaprogramming virtual - "overload" in implementation
        { // only SampleType* variables here
            const SampleType *buffer = dsp::tables::DSP[0];
        } I;
        static constexpr const char *I_names[] = {
            "Buffer * --> start of sample buffer of size 2^BufferBits"
        }; // indirect parameter names
    };

    DECL_WAVE(buffer)

    static CoeffType default_ratios[1] = { 0 };
    static CoeffType default_gains[1] = { 0 };

    template <typename Base = buffer_wave>
    class formant_logic : public logic_base<op1<Base>>
    {
    public:
        static constexpr auto signature = uti::STR2A("Formant") + op1<Base>::signature;
        inline void setup() // metaprogramming virtual function - "overload" in implementation
        {
            this->gather_mask = 0;
            this->advance_mask = 0;
        }

        inline SampleType postSampling()
        {
            SampleType sample = 0;
            for (SampleType i = 0; i < S.count; ++i)
            {
                this->ops.o1.logic.S.phase = fpm::mul<PhaseDescr,CoeffDescr,PhaseDescr>(I.ratios[i], S.phase);
                sample += fpm::mul<SampleDescr,SampleDescr,CoeffDescr>(this->ops.o1.getSample(), I.gains[i]);
            }
            return sample;
        }

        inline void postAdvance() // metaprogramming virtual function - "overload" in implementation
        {
            S.phase += S.increment;
        }

        struct // scratchpad  // metaprogramming virtual - "overload" in implementation
        { // only SampleType variables here
            PhaseType increment = 0;
            PhaseType phase = 0;
            SampleType count = 0;
        } S;
        static constexpr const char *S_names[] = { 
            "Increment [0:unsigned Max] --> [0:1[ normalized frequency",
            "Phase [Min:Max] --> [SampleMin:SampleMax[ current sample",
            "Count [0:unsigned Max] --> [0:SampleMax[ number of formants"
        }; // scratchpad parameter names

        struct // indirect // metaprogramming virtual - "overload" in implementation
        { // only SampleType* variables here
            CoeffType *ratios = default_ratios;
            CoeffType *gains = default_gains;
        } I;
        static constexpr const char *I_names[] = {
            "Ratios * --> array of formant frequency ratios",
            "Gains * --> array of formant gains"	
        }; // indirect parameter names
    };

    template <typename Base = buffer_wave>
    DECL_LOGIC_WAVE_T(formant, Base)

    template <typename Base= buffer_wave>
    DECL_SCRIPTABLE_WAVE_T(formant, Base)
}