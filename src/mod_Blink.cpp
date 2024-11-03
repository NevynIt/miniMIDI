#include "mod_Blink.h"
#include "pico/stdlib.h"

#include "stdio.h"
#include "App.h"

void mod_Blink::Init() {
    gpio_init(GPIO_Board_Led);
    gpio_set_dir(GPIO_Board_Led, GPIO_OUT);
    for (int i = 0; i < 5; i++) { //Quickly blink 5 times to show the system is alive
        ledState = !ledState;
        sleep_ms(200);
        gpio_put(GPIO_Board_Led, ledState);
    }
}

void mod_Blink::Tick() {
    INTERVALCHECK(BLINK_MS)
    // {
    //     static uint32_t INTERVALCHECK_stored_time = 0;
    //     if (time_us_32()-INTERVALCHECK_stored_time > BLINK_MS * 1000)
    //     {
    //         INTERVALCHECK_stored_time = time_us_32();
    //     }
    //     else
    //     {
    //         return;
    //     }
    // }
    Toggle();
}

void mod_Blink::Toggle()
{
    ledState = !ledState;
    gpio_put(GPIO_Board_Led, ledState);
}
