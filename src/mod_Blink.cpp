#include "mod_Blink.h"
#include "pico/stdlib.h"

#include "stdio.h"

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
    uint32_t currentTime = to_ms_since_boot(get_absolute_time());
    if (currentTime - lastTickTime >= BLINK_MS) {
        ledState = !ledState;
        gpio_put(GPIO_Board_Led, ledState);
        lastTickTime = currentTime;
    }
}
