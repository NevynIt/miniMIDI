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

void disp_inputs()
{
    app.display.clear();
    char buffer[100];
    sprintf(buffer, "%d, %d, %d, %d, %d\n", app.encoders.buttons, app.encoders.count[0], app.encoders.count[1], app.encoders.count[2], app.encoders.count[3]);
    app.display.draw_text(0, 16, 1, buffer);
    sprintf(buffer, "%d, %d, %d\n", app.joys.button, app.joys.GetX(), app.joys.GetY());
    app.display.draw_text(0, 24, 1, buffer);
}

int main()
{
    set_sys_clock_khz(CPU_KHZ, true);

    // Initialise the modules on core 0 first
    app.Init_c0();

    multicore_launch_core1(core1_entry);

    while (true)
    {
        app.Tick_c0();
        disp_inputs();
        // app.ledStrip.Test();
        app.usbAudio.Test();
    }

    return 0;
}
