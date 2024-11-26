#include "App.h"
#include "pico/multicore.h"
#include "hwConfig.h"
#include "hardware/clocks.h"
#include "stdio.h"

void core1_entry()
{
    multicore_lockout_victim_init();
    mMApp.Init_c1();

    while (true)
    {
        mMApp.Tick_c1();
    }
}

int main()
{
    set_sys_clock_khz(CPU_KHZ, true);
    multicore_lockout_victim_init();
    // Initialise the modules on core 0 first
    mMApp.Init_c0();

    multicore_launch_core1(core1_entry);

    while (true)
    {
        mMApp.Tick_c0();
    }

    return 0;
}
