#include "mod_Blink.h"
#include "pico/stdlib.h"

#include "stdio.h"
#include "App.h"

void mod_Blink::Init() {
    gpio_init(GPIO_Board_Led);
    gpio_set_dir(GPIO_Board_Led, GPIO_OUT);
    for (int i = 0; i < 6; i++) { //Quickly blink 3 times to show the system is alive
        ledState = !ledState;
        sleep_ms(200);
        gpio_put(GPIO_Board_Led, ledState);
    }
}

void mod_Blink::Tick() {
    INTERVALCHECK(interval)

    Toggle();
}

void mod_Blink::Toggle()
{
    ledState = !ledState;
    gpio_put(GPIO_Board_Led, ledState);
}
