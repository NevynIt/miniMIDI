#include "mod_Blink.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "wavetables/WT_BASE.h"
#include "hardware/structs/pwm.h"

#include "stdio.h"
#include "App.h"

DEOPTIMIZE
void mod_Blink::Init() {
    gpio_init(GPIO_Board_Led);
    gpio_set_dir(GPIO_Board_Led, GPIO_OUT);
    for (int i = 0; i < 6; i++) { //Quickly blink 3 times to show the system is alive
        ledState = !ledState;
        sleep_ms(200);
        gpio_put(GPIO_Board_Led, ledState);
    }
    // Initialize PWM on LED GPIO pin
    gpio_set_function(GPIO_Board_Led, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(GPIO_Board_Led);
    pwm_set_wrap(slice_num, 255); // Set PWM period
    pwm_set_enabled(slice_num, true);
    pwm_set_gpio_level(GPIO_Board_Led, 128);

    // ledWave.increment = fp_index(freq / BLINK_REFRESH_HZ);
    // printf("Blink frequency: %d\n", ledWave.increment.raw_value());
}

DEOPTIMIZE
void mod_Blink::Tick() {
    INTERVALCHECK(1000.0f / BLINK_REFRESH_HZ)

    // Toggle();

    // Update PWM duty cycle based on wave sample
    ledWave.increment = fp_index(freq * (1000.0f / BLINK_REFRESH_HZ));
    sample_t sample = ledWave.getSample();
    uint16_t duty = (float(sample) - SAMPLE_MIN) / ((float)SAMPLE_MAX - SAMPLE_MIN) * level;
    pwm_set_gpio_level(GPIO_Board_Led, duty);
}

void mod_Blink::Toggle()
{
    ledState = !ledState;
    gpio_put(GPIO_Board_Led, ledState);
}

void mod_Blink::setWaveform(sample_cptr waveform) {
    ledWave.waveform = waveform;
}
