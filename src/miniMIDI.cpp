#include "App.h"
#include "pico/multicore.h"
#include "hwConfig.h"
#include "hardware/clocks.h"
#include "stdio.h"

void core1_entry()
{
    app.Init_c1();

    while (true)
    {
        app.Tick_c1();
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

// template<class T>
// int ilog2(T x)
// {
//     return (sizeof(T)*8 -1 ) - __builtin_clz(abs(x));
// }

// #include <random>
// void test()
// {
//     int x = rand();
//     printf("log2(%d) = %d\n", x, ilog2(x));
// }

int main()
{
    set_sys_clock_khz(CPU_KHZ, true);

    // Initialise the modules on core 0 first
    app.Init_c0();

    multicore_launch_core1(core1_entry);

    
    auto p = fp_int(0x2342).to_signed();
    

    while (true)
    {
        app.Tick_c0();
        // test();
    }

    return 0;
}
