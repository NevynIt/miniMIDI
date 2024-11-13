#include "mod_Blink.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/structs/pwm.h"

#include "stdio.h"
#include "App.h"

void mod_Blink::Init() {
    // Initialize PWM on LED GPIO pin
    gpio_set_function(GPIO_Board_Led, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(GPIO_Board_Led);
    pwm_set_wrap(slice_num, 255); // Set PWM period
    pwm_set_enabled(slice_num, true);

    int level = 32;
    for (int i = 0; i < 6; i++) { //Quickly blink 3 times to show the system is alive
        sleep_ms(200);
        pwm_set_gpio_level(GPIO_Board_Led, level);
        level = 255 - level;
    }

    ledWave.setLevel(128);
}

void mod_Blink::Tick() {
    INTERVALCHECK(1000.0f / BLINK_REFRESH_HZ)

    // Update PWM duty cycle based on wave sample
    ledWave.setFrequency(freq, BLINK_REFRESH_HZ);
    ledWave.advance();
    uint16_t duty = ledWave.getSample() + 128;
    pwm_set_gpio_level(GPIO_Board_Led, duty);
}