#include "mod_Joys.h"

void mod_Joys::Init() {
    // Initialize the ADC
    adc_init();
    adc_gpio_init(GPIO_Joys_X);
    adc_gpio_init(GPIO_Joys_Y);

    gpio_init(GPIO_Joys_Btn);
    gpio_set_dir(GPIO_Joys_Btn, GPIO_IN);
    gpio_pull_up(GPIO_Joys_Btn);
}

void mod_Joys::Tick() {
    // Read X axis
    adc_select_input(ADC_Joys_X);
    xValue = adc_read();

    // Read Y axis
    adc_select_input(ADC_Joys_Y);
    yValue = adc_read();

    // Read button
    button = !gpio_get(GPIO_Joys_Btn);
}

void mod_Joys::Test()
{
    
}
