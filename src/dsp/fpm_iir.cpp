// IIR can be used with some performance redefining the pieces that are used by the "filter" function
// that is:
//  - within Biquad class, the member variables and the filter function
//  - within the CascadeStages class, the filter function and the biquad member variables
// I think it's possible to create a super lightweight version of the filter function that just copies the coefficient from the IIR implementation
// identification of parameters will be slow but precise (using double precision)
// execution can be fast (using fixed precision math)
#include "iir.h"
#include "fpm_iir.h"
#include "hardware/clocks.h"

using fp_int = fpm::fixed<16, 16>;
using namespace Iir;

void test()
{
    // Create a Butterworth lowpass filter
    using filter = Iir::ChebyshevI::BandPass<4>;
    // using filter = Iir::Butterworth::LowPass<2>;
    // using filter = Iir::RBJ::LowPass;
    filter f;
    // Set the cutoff frequency to 0.2 of Nyquist
    f.setup(2, 0.2, 0.1, 5);
    // f.setup(2, 0.2);
    // f.setup(0.2, 0.05);

    using sampT = float;
    using coefT = float;

    // fpmRBJFilter fpmF(f);
    // fpmRBJFilter<sampT, coefT> fpmF2(f);

    fpmFilter<filter> fpmF(f);
    fpmFilter<filter, sampT, coefT> fpmF2(f);

    // Filter some data
    fp_int fp_x(0);
    double x = 0;
    sampT fp2_x = sampT(0);

    for (int i = 0; i < 200; ++i)
    {
        fp_int fp_y = fpmF.filter(fp_x);
        double y = f.filter(x);
        sampT fp2_y = fpmF2.filter(fp2_x);
        printf("%10.6f  %10.6f  %10.6f \n", y, (double)fp_y, (double)fp2_y);
        x = (i == 10) ? 1 : 0;
        fp_x = x;
        fp2_x = x;
    }

    const auto reps = 5000;

    //test speed of the two implementations
    uint32_t start = time_us_32();
    for (int i = 0; i < reps; ++i)
    {
        fpmF.filter(fp_x);
    }
    printf("fpm: %d\n", time_us_32() - start);

    start = time_us_32();
    for (int i = 0; i < reps; ++i)
    {
        f.filter(x);
    }
    printf("f: %d\n", time_us_32() - start);

    start = time_us_32();
    for (int i = 0; i < reps; ++i)
    {
        fpmF2.filter(fp2_x);
    }
    printf("fpm2: %d\n", time_us_32() - start);
}
