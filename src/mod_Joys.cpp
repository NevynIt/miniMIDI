#include "mod_Joys.h"

void mod_Joys::Init() {
    // Initialize the ADC
    adc_init();
    adc_gpio_init(GPIO_Joys_X);
    adc_gpio_init(GPIO_Joys_Y);
}

void mod_Joys::Tick() {
    // Read X axis
    adc_select_input(ADC_Joys_X);
    xValue = adc_read();

    // Read Y axis
    adc_select_input(ADC_Joys_Y);
    yValue = adc_read();
}

uint16_t mod_Joys::GetX() {
    return xValue;
}

uint16_t mod_Joys::GetY() {
    return yValue;
}