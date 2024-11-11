#pragma once

#include "iir.h"
#include "fpm/fixed.hpp"
#include <type_traits>

template <typename CoefficientsType>
class fpmBiquad
{
    public:
    fpmBiquad() = default;

    void set(Iir::Biquad src)
    {
        m_a0 = src.getA0();
        m_a1 = src.getA1();
        m_a2 = src.getA2();
        m_b0 = src.getB0();
        m_b1 = src.getB1();
        m_b2 = src.getB2();
    }

    CoefficientsType m_a0 = CoefficientsType(0);
	CoefficientsType m_a1 = CoefficientsType(0);
	CoefficientsType m_a2 = CoefficientsType(0);
	CoefficientsType m_b1 = CoefficientsType(0);
	CoefficientsType m_b2 = CoefficientsType(0);
	CoefficientsType m_b0 = CoefficientsType(0);
};

template <typename SampleType, typename CoefficientsType, class StateType>
class fpmState
{
};

template <typename SampleType, typename CoefficientsType>
class fpmState<SampleType, CoefficientsType, Iir::DirectFormI>
{
	public:
    fpmState() = default;

	fpmState(const Iir::DirectFormI &src) { set(src); }
	
	void set(const Iir::DirectFormI &src)
	{
        m_x2 = src.m_x2;
        m_y2 = src.m_y2;
        m_x1 = src.m_x1;
        m_y1 = src.m_y1;
	}
	
	inline SampleType filter(const SampleType in,
			     const fpmBiquad<CoefficientsType>& s)
	{
		const SampleType out = s.m_b0*in + s.m_b1*m_x1 + s.m_b2*m_x2
		- s.m_a1*m_y1 - s.m_a2*m_y2;
		m_x2 = m_x1;
		m_y2 = m_y1;
		m_x1 = in;
		m_y1 = out;
		
		return out;
	}
	
protected:
	SampleType m_x2  = SampleType(0); // x[n-2]
	SampleType m_y2  = SampleType(0); // y[n-2]
	SampleType m_x1  = SampleType(0); // x[n-1]
	SampleType m_y1  = SampleType(0); // y[n-1]
};

template <typename SampleType, typename CoefficientsType>
class fpmState<SampleType, CoefficientsType, Iir::DirectFormII>
{
    public:
    fpmState() = default;

	fpmState(const Iir::DirectFormII &src) { set(src); }

    void set(const Iir::DirectFormII &src)
    {
        m_v1 = src.m_v1;
        m_v2 = src.m_v2;
    }

    inline SampleType filter(const SampleType in,
                         const fpmBiquad<CoefficientsType>& s)
    {
        const SampleType w   = in - s.m_a1 * m_v1 - s.m_a2 * m_v2;
        const SampleType out =      s.m_b0 * w    + s.m_b1 * m_v1 + s.m_b2 * m_v2;

        m_v2 = m_v1;
        m_v1 = w;

        return out;
    }

protected:
    SampleType m_v1 = SampleType(0); // v[n-1]
    SampleType m_v2 = SampleType(0); // v[n-2]
};

template <typename SampleType, typename CoefficientsType>
class fpmState<SampleType, CoefficientsType, Iir::TransposedDirectFormII>
{
    public:
    fpmState() = default;

	fpmState(const Iir::TransposedDirectFormII &src) { set(src); }

    void set(const Iir::TransposedDirectFormII &src)
    {
        m_s1 = src.m_s1;
        m_s1_1 = src.m_s1_1;
        m_s2 = src.m_s2;
        m_s2_1 = src.m_s2_1;
    }

    inline SampleType filter(const SampleType in,
                         const fpmBiquad<CoefficientsType>& s)
    {
        const SampleType out = m_s1_1 + s.m_b0 * in;
        m_s1 = m_s2_1 + s.m_b1 * in - s.m_a1 * out;
        m_s2 = s.m_b2 * in - s.m_a2 * out;
        m_s1_1 = m_s1;
        m_s2_1 = m_s2;

        return out;
    }

protected:
    SampleType m_s1  = SampleType(0);
    SampleType m_s1_1  = SampleType(0);
    SampleType m_s2  = SampleType(0);
    SampleType m_s2_1  = SampleType(0);
};

template <class Filter, typename SampleType = fpm::fixed<16,16>, typename CoefficientsType = fpm::fixed<2,30>>
class fpmFilter
{
    static const int MaxStages = Filter::template_MaxStages;
    using StateType = typename Filter::template_StateType;
    using fpmStateType = fpmState<SampleType, CoefficientsType, StateType>;

public:
    fpmFilter() = default;

    fpmFilter(const Filter &src)
    {
        set(src);
    }

    void set(const Filter &src)
    {
        for (int i = 0; i < MaxStages; i++)
            m_stages[i].set(src.m_stages[i]);
    }

    template <typename Sample>
    inline Sample filter(const Sample in)
    {
            SampleType out = in;
            fpmStateType* state = m_states;
            for (const auto &stage: m_stages)
                    out = (state++)->filter(out, stage);
            return static_cast<Sample> (out);
    }
    
private:
    fpmBiquad<CoefficientsType> m_stages[MaxStages];
    fpmStateType m_states[MaxStages];
};


template <typename SampleType = fpm::fixed<16,16>, typename CoefficientsType = fpm::fixed<2,30>>
class fpmRBJFilter
{
    public:
    fpmRBJFilter() = default;

    fpmRBJFilter(const Iir::RBJ::RBJbase &src)
    {
        set(src);
    }

    void set(const Iir::RBJ::RBJbase &src)
    {
        m_biquad.set(src);
    }

    template <typename Sample>
        inline Sample filter(Sample s) {
        return static_cast<Sample>(m_state.filter((SampleType)s, m_biquad));
    }

    private:
    fpmState<SampleType, CoefficientsType, Iir::DirectFormI> m_state;
    fpmBiquad<CoefficientsType> m_biquad;
};
