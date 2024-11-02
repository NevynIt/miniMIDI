#include "mod_UART.h"
#include "msg_MIDI.h"
#include "hardware/uart.h"
#include "pico/stdio.h"
#include "stdio.h"

void mod_UART::Init() {
    // Initialize UART
    gpio_set_function(GPIO_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(GPIO_UART_RX, GPIO_FUNC_UART);
    uart_init(UART_INST, UART_BAUD_RATE);

#if UART_USE == 1
    // Make the UART pins available to picotool
    stdio_init_all();
    printf("\n\n\nUART initialized\n");
#endif
}

void mod_UART::Tick() {
    
}