#include "types.h"

//nice: http://jaggedplanet.com/iir/iir-explorer.asp
//https://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/
//https://www.micromodeler.com/dsp/

namespace dsp
{
    #define ONESQRT2 (1/sqrt(2))

    inline constexpr flp_t normalizeFreq(flp_t freq, flp_t sampleRate)
    {
        return freq / sampleRate;
    }

	class RBJ
	{
	public:
        RBJ() { identity(); };

        inline SampleType filter(const SampleType x)
        {
            using tmpDescr = ::fpm::mul_result_struct<StateDescr, CoeffDescr>;
            using ee = tmpDescr::t;
            const StateType in = ::fpm::conv<StateDescr, SampleDescr>(x);
            
            ee tmp = (ee)m_b0*in 
                   + (ee)m_b1*m_x1 
                   + (ee)m_b2*m_x2 
                   - (ee)m_a1*m_y1 
                   - (ee)m_a2*m_y2;
        
            const StateType out = ::fpm::conv<StateDescr, tmpDescr>(tmp);
            m_x2 = m_x1;
            m_y2 = m_y1;
            m_x1 = in;
            m_y1 = out;
            
            return ::fpm::conv<SampleDescr, StateDescr>(out);
        }

        /**
         * Sets all coefficients
         * \param a0 1st IIR coefficient
         * \param a1 2nd IIR coefficient
         * \param a2 3rd IIR coefficient
         * \param b0 1st FIR coefficient
         * \param b1 2nd FIR coefficient
         * \param b2 3rd FIR coefficient
         **/
        void setCoefficients (flp_t a0, flp_t a1, flp_t a2,flp_t b0, flp_t b1, flp_t b2);

        /**
         * Sets the coefficiens as pass through. (b0=1,a0=1, rest zero)
         **/
        void identity ();

        /**
         * Performs scaling operation on the FIR coefficients
         * \param scale Mulitplies the coefficients b0,b1,b2 with the scaling factor scale.
         **/
        // void applyScale (flp_t scale);

        /**
         * Lowpass.
         * \param cutoffFrequency Normalised cutoff frequency
         * \param q Q factor determines the resonance peak at the cutoff.
         **/
        void lowpass(flp_t cutoffFrequency, flp_t q = ONESQRT2);

        /**
         * Highpass.
         * \param cutoffFrequency Normalised cutoff frequency (0..1/2)
         * \param q Q factor determines the resonance peak at the cutoff.
         **/
        void highpass(flp_t cutoffFrequency, flp_t q = ONESQRT2);

        /**
         * Bandpass with constant skirt gain
         * \param centerFrequency Center frequency of the bandpass
         * \param bandWidth Bandwidth in octaves
         **/
        void bandpass1(flp_t centerFrequency, flp_t bandWidth);

        /**
         * Bandpass with constant 0 dB peak gain
         * \param centerFrequency Normalised centre frequency of the bandpass
         * \param bandWidth Bandwidth in octaves
         **/
        void bandpass2(flp_t centerFrequency, flp_t bandWidth);

        /**
         * Bandstop filter. Warning: the bandwidth might not be accurate
         * for narrow notches.
         * \param centerFrequency Normalised centre frequency of the bandpass
         * \param bandWidth Bandwidth in octaves
         **/
        void bandstop(flp_t centerFrequency, flp_t bandWidth);

        /**
         * Bandstop with Q factor: the higher the Q factor the more narrow is
         * the notch. 
         * However, a narrow notch has a long impulse response ( = ringing)
         * and numerical problems might prevent perfect damping. Practical values
         * of the Q factor are about Q = 10 to 20. In terms of the design
         * the Q factor defines the radius of the
         * poles as r = exp(- pi*(centerFrequency/sampleRate)/q_factor) whereas
         * the angles of the poles/zeros define the bandstop frequency. The higher
         * Q the closer r moves towards the unit circle.
         * \param centerFrequency Normalised centre frequency of the notch
         * \param q_factor Q factor of the notch (1 to ~20)
        **/
        void notch(flp_t centerFrequency, flp_t q_factor = 10);

        /**
         * Low shelf: 0db in the stopband and gainDb in the passband.
         * \param cutoffFrequency Normalised cutoff frequency
         * \param gainDb Gain in the passband
         * \param shelfSlope Slope between stop/passband. 1 = as steep as it can.
         **/
        void lowshelf(flp_t cutoffFrequency, flp_t gainDb, flp_t shelfSlope = 1);

        /**
         * High shelf: 0db in the stopband and gainDb in the passband.
         * \param cutoffFrequency Normalised cutoff frequency
         * \param gainDb Gain in the passband
         * \param shelfSlope Slope between stop/passband. 1 = as steep as it can.
         **/
        void highshelf(flp_t cutoffFrequency, flp_t gainDb, flp_t shelfSlope = 1);

        /**
         * Band shelf: 0db in the stopband and gainDb in the passband.
         * \param centerFrequency Normalised centre frequency
         * \param gainDb Gain in the passband
         * \param bandWidth Bandwidth in octaves
         **/
        void bandshelf(flp_t centerFrequency, flp_t gainDb, flp_t bandWidth);

        /**
         * Allpass filter.
         * \param phaseFrequency Normalised frequency where the phase flips
         * \param q Q-factor
         **/
        void allpass(flp_t phaseFrequency, flp_t q = ONESQRT2);

        CoeffType m_a0;
        CoeffType m_a1;
        CoeffType m_a2;
        CoeffType m_b1;
        CoeffType m_b2;
        CoeffType m_b0;

        void reset ()
        {
            m_x1 = 0;
            m_x2 = 0;
            m_y1 = 0;
            m_y2 = 0;
        }
        
        StateType m_x2 = 0; // x[n-2]
        StateType m_y2 = 0; // y[n-2]
        StateType m_x1 = 0; // x[n-1]
        StateType m_y1 = 0; // y[n-1]
    };
}