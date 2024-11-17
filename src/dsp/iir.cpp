#include "iir.h"
#include <math.h>

namespace dsp
{
    const flp_t flp_tPi	   = 3.1415926535897932384626433832795028841971;
    const flp_t flp_tPi_2  = 1.5707963267948966192313216916397514420986;
    const flp_t flp_tLn2   = 0.69314718055994530941723212145818;
    const flp_t flp_tLn10  = 2.3025850929940456840179914546844;

    #define throw_invalid_argument(msg) 
    // #define throw_invalid_argument(msg) __breakpoint()

    template <typename Ty>
    inline bool is_nan (Ty v)
    {
        return !(v == v);
    }

	void RBJ::setCoefficients (flp_t a0, flp_t a1, flp_t a2, flp_t b0, flp_t b1, flp_t b2)
	{
		if (is_nan (a0)) throw_invalid_argument("a0 is NaN");
		if (is_nan (a1)) throw_invalid_argument("a1 is NaN");
		if (is_nan (a2)) throw_invalid_argument("a2 is NaN");
		if (is_nan (b0)) throw_invalid_argument("b0 is NaN");
		if (is_nan (b1)) throw_invalid_argument("b1 is NaN");
		if (is_nan (b2)) throw_invalid_argument("b2 is NaN");

		//check statically if fpl_t is float
		if (sizeof(flp_t) == sizeof(float))
		{
			m_a0 = fpm::from_float<CoeffDescr>(a0);
			m_a1 = fpm::from_float<CoeffDescr>(a1/a0);
			m_a2 = fpm::from_float<CoeffDescr>(a2/a0);
			m_b0 = fpm::from_float<CoeffDescr>(b0/a0);
			m_b1 = fpm::from_float<CoeffDescr>(b1/a0);
			m_b2 = fpm::from_float<CoeffDescr>(b2/a0);
		}
		else if (sizeof(flp_t) == sizeof(double))
		{
			m_a0 = fpm::from_double<CoeffDescr>(a0);
			m_a1 = fpm::from_double<CoeffDescr>(a1/a0);
			m_a2 = fpm::from_double<CoeffDescr>(a2/a0);
			m_b0 = fpm::from_double<CoeffDescr>(b0/a0);
			m_b1 = fpm::from_double<CoeffDescr>(b1/a0);
			m_b2 = fpm::from_double<CoeffDescr>(b2/a0);
		}
		else
		{
			throw_invalid_argument("flp_t is not float or double");
		}
	}

    void RBJ::identity ()
	{
		setCoefficients (1, 0, 0, 1, 0, 0);
	}

	// void RBJ::applyScale (flp_t scale)
	// {
	// 	m_b0 *= scale;
	// 	m_b1 *= scale;
	// 	m_b2 *= scale;
	// }

	void RBJ::lowpass(flp_t cutoffFrequency, flp_t q)
	{
		flp_t w0 = 2 * flp_tPi * cutoffFrequency;
		flp_t cs = cos (w0);
		flp_t sn = sin (w0);
		flp_t AL = sn / (2 * q);
		flp_t b0 = (1 - cs) / 2;
		flp_t b1 =  1 - cs;
		flp_t b2 = (1 - cs) / 2;
		flp_t a0 =  1 + AL;
		flp_t a1 = -2 * cs;
		flp_t a2 =  1 - AL;
		setCoefficients (a0, a1, a2, b0, b1, b2);
	}

	void RBJ::highpass (flp_t cutoffFrequency, flp_t q)
	{
		flp_t w0 = 2 * flp_tPi * cutoffFrequency;
		flp_t cs = cos (w0);
		flp_t sn = sin (w0);
		flp_t AL = sn / ( 2 * q );
		flp_t b0 =  (1 + cs) / 2;
		flp_t b1 = -(1 + cs);
		flp_t b2 =  (1 + cs) / 2;
		flp_t a0 =  1 + AL;
		flp_t a1 = -2 * cs;
		flp_t a2 =  1 - AL;
		setCoefficients (a0, a1, a2, b0, b1, b2);
	}

	void RBJ::bandpass1 (flp_t centerFrequency, flp_t bandWidth)
	{
		flp_t w0 = 2 * flp_tPi * centerFrequency;
		flp_t cs = cos (w0);
		flp_t sn = sin (w0);
		flp_t AL = sn / ( 2 * bandWidth );
		flp_t b0 = bandWidth * AL;// sn / 2;
		flp_t b1 =  0;
		flp_t b2 = -bandWidth * AL;//-sn / 2;
		flp_t a0 =  1 + AL;
		flp_t a1 = -2 * cs;
		flp_t a2 =  1 - AL;
		setCoefficients (a0, a1, a2, b0, b1, b2);
	}
	
	void RBJ::bandpass2 (flp_t centerFrequency, flp_t bandWidth)
	{
		flp_t w0 = 2 * flp_tPi * centerFrequency;
		flp_t cs = cos (w0);
		flp_t sn = sin (w0);
		flp_t AL = sn / ( 2 * bandWidth );
		flp_t b0 =  AL;
		flp_t b1 =  0;
		flp_t b2 = -AL;
		flp_t a0 =  1 + AL;
		flp_t a1 = -2 * cs;
		flp_t a2 =  1 - AL;
		setCoefficients (a0, a1, a2, b0, b1, b2);
	}
	
	void RBJ::bandstop (flp_t centerFrequency, flp_t bandWidth)
	{
		flp_t w0 = 2 * flp_tPi * centerFrequency;
		flp_t cs = cos (w0);
		flp_t sn = sin (w0);
		flp_t AL = sn / ( 2 * bandWidth );
		flp_t b0 =  1;
		flp_t b1 = -2 * cs;
		flp_t b2 =  1;
		flp_t a0 =  1 + AL;
		flp_t a1 = -2 * cs;
		flp_t a2 =  1 - AL;
		setCoefficients (a0, a1, a2, b0, b1, b2);
	}
	
	void RBJ::notch (flp_t centerFrequency, flp_t q_factor)
	{
		flp_t w0 = 2 * flp_tPi * centerFrequency;
		flp_t cs = cos (w0);
		flp_t r = exp(-(w0/2) / q_factor);
		flp_t b0 =  1;
		flp_t b1 = -2 * cs;
		flp_t b2 =  1;
		flp_t a0 =  1;
		flp_t a1 = -2 * r * cs;
		flp_t a2 =  r * r;
		setCoefficients (a0, a1, a2, b0, b1, b2);
	}
	
	void RBJ::lowshelf (flp_t cutoffFrequency, flp_t gainDb, flp_t shelfSlope)
	{
		flp_t A  = pow (10, gainDb/40);
		flp_t w0 = 2 * flp_tPi * cutoffFrequency;
		flp_t cs = cos (w0);
		flp_t sn = sin (w0);
		flp_t AL = sn / 2 * ::std::sqrt ((A + 1/A) * (1/shelfSlope - 1) + 2);
		flp_t sq = 2 * sqrt(A) * AL;
		flp_t b0 =    A*( (A+1) - (A-1)*cs + sq );
		flp_t b1 =  2*A*( (A-1) - (A+1)*cs );
		flp_t b2 =    A*( (A+1) - (A-1)*cs - sq );
		flp_t a0 =        (A+1) + (A-1)*cs + sq;
		flp_t a1 =   -2*( (A-1) + (A+1)*cs );
		flp_t a2 =        (A+1) + (A-1)*cs - sq;
		setCoefficients (a0, a1, a2, b0, b1, b2);
	}
		
	void RBJ::highshelf (flp_t cutoffFrequency, flp_t gainDb, flp_t shelfSlope)
	{
		flp_t A  = pow (10, gainDb/40);
		flp_t w0 = 2 * flp_tPi * cutoffFrequency;
		flp_t cs = cos (w0);
		flp_t sn = sin (w0);
		flp_t AL = sn / 2 * ::std::sqrt ((A + 1/A) * (1/shelfSlope - 1) + 2);
		flp_t sq = 2 * sqrt(A) * AL;
		flp_t b0 =    A*( (A+1) + (A-1)*cs + sq );
		flp_t b1 = -2*A*( (A-1) + (A+1)*cs );
		flp_t b2 =    A*( (A+1) + (A-1)*cs - sq );
		flp_t a0 =        (A+1) - (A-1)*cs + sq;
		flp_t a1 =    2*( (A-1) - (A+1)*cs );
		flp_t a2 =        (A+1) - (A-1)*cs - sq;
		setCoefficients (a0, a1, a2, b0, b1, b2);
	}
	
	void RBJ::bandshelf (flp_t centerFrequency, flp_t gainDb, flp_t bandWidth)
	{
		flp_t A  = pow (10, gainDb/40);
		flp_t w0 = 2 * flp_tPi * centerFrequency;
		flp_t cs = cos(w0);
		flp_t sn = sin(w0);
		flp_t AL = sn * sinh( flp_tLn2/2 * bandWidth * w0/sn );
		if (is_nan (AL))
			throw_invalid_argument("No solution available for these parameters.\n");
		flp_t b0 =  1 + AL * A;
		flp_t b1 = -2 * cs;
		flp_t b2 =  1 - AL * A;
		flp_t a0 =  1 + AL / A;
		flp_t a1 = -2 * cs;
		flp_t a2 =  1 - AL / A;
		setCoefficients (a0, a1, a2, b0, b1, b2);
	}
	
    void RBJ::allpass (flp_t phaseFrequency, flp_t q)
    {
        flp_t w0 = 2 * flp_tPi * phaseFrequency;
        flp_t cs = cos (w0);
        flp_t sn = sin (w0);
        flp_t AL = sn / ( 2 * q );
        flp_t b0 =  1 - AL;
        flp_t b1 = -2 * cs;
        flp_t b2 =  1 + AL;
        flp_t a0 =  1 + AL;
        flp_t a1 = -2 * cs;
        flp_t a2 =  1 - AL;
        setCoefficients (a0, a1, a2, b0, b1, b2);
    }

}
