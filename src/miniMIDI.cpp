#include "App.h"
#include "pico/multicore.h"
#include "hwConfig.h"
#include "hardware/clocks.h"
#include "stdio.h"

void core1_entry()
{
    mMApp.Init_c1();

    while (true)
    {
        mMApp.Tick_c1();
    }
}

// extern "C"
// {
// int i2s_example_main();
// }

// int main() {

//     i2s_example_main();
//     return 0;
// }

// IIR can be used with some performance redefining the pieces that are used by the "filter" function
// that is:
//  - within Biquad class, the member variables and the filter function
//  - within the CascadeStages class, the filter function and the biquad member variables
// I think it's possible to create a super lightweight version of the filter function that just copies the coefficient from the IIR implementation
// identification of parameters will be slow but precise (using double precision)
// execution can be fast (using fixed precision math)
#define IIR1_NO_EXCEPTIONS
#include "iir.h"

void test()
{
    // Create a Butterworth lowpass filter
    Iir::ChebyshevI::BandPass<4> f;
    // Set the cutoff frequency to 0.2 of Nyquist
    f.setup(2, 0.3, 0.05, 10);

    // Filter some data
    double x = 0;
    for (int i = 0; i < 100; ++i)
    {
        double y = f.filter(x);
        printf("%10.6f\n", y);
        x = (i == 10) ? 1 : 0;
    }
}

int main()
{
    set_sys_clock_khz(CPU_KHZ, true);

    // Initialise the modules on core 0 first
    mMApp.Init_c0();

    test();

    multicore_launch_core1(core1_entry);

    while (true)
    {
        mMApp.Tick_c0();
        // test();
    }

    return 0;
}
