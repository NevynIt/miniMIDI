#include "mod_Blink.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/structs/pwm.h"

#include "stdio.h"
#include "App.h"

void mod_Blink::Init() {
    // Initialize PWM on LED GPIO pin
    gpio_set_function(mMApp.hwConfig->gpio_blink_led, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(mMApp.hwConfig->gpio_blink_led);
    pwm_set_wrap(slice_num, 255); // Set PWM period
    pwm_set_enabled(slice_num, true);

    int level = 32;
    for (int i = 0; i < 6; i++) { //Quickly blink 3 times to show the system is alive
        sleep_ms(200);
        pwm_set_gpio_level(mMApp.hwConfig->gpio_blink_led, level);
        level = 255 - level;
    }
}

void mod_Blink::Tick() {
    INTERVALCHECK(mMApp.hwConfig->blink_refresh_ms);

    if (ledWave == nullptr) {
        auto w = new dsp::sinWave();
        w->setIncrement(dsp::inc_from_freq(1, 1000/mMApp.hwConfig->blink_refresh_ms));
        ledWave = w;
    } else {
        ledWave->operator++();
    }
    // Update PWM duty cycle based on wave sample
    uint16_t duty = fpm::mul<fpm::int8_descr,fpm::int8_descr, dsp::LevelDescr>(128, (*ledWave)())+128;
    pwm_set_gpio_level(mMApp.hwConfig->gpio_blink_led, duty);
}

void mod_Blink::Toggle() {
    static bool on = false;
    on = !on;
    pwm_set_gpio_level(mMApp.hwConfig->gpio_blink_led, on ? 255-32 : 32);
}