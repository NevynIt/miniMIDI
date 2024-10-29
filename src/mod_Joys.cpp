#include "mod_Joys.h"

void mod_Joys::Init() {
    // Initialize the ADC
    adc_init();
    adc_gpio_init(JOY_X_PIN);
    adc_gpio_init(JOY_Y_PIN);
}

void mod_Joys::Tick() {
    // Read X axis
    adc_select_input(0);
    xValue = adc_read();

    // Read Y axis
    adc_select_input(1);
    yValue = adc_read();
}

uint16_t mod_Joys::GetX() {
    return xValue;
}

uint16_t mod_Joys::GetY() {
    return yValue;
}