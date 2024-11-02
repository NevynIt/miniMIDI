#include "mod_UART.h"
#include "msg_MIDI.h"
#include "hardware/uart.h"
#include "pico/stdio.h"
#include "stdio.h"

extern "C" {
    void stdio_uart_init_full(struct uart_inst *uart, uint baud_rate, int tx_pin, int rx_pin);
}

void mod_UART::Init() {
#if LIB_PICO_STDIO_UART
    // Make the UART pins available to picotool
    stdio_uart_init_full(UART_INST, UART_BAUD_RATE, GPIO_UART_TX, GPIO_UART_RX);
    printf("\n\n\nUART STDIO initialized\n");
#else
    // Initialize UART
    gpio_set_function(GPIO_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(GPIO_UART_RX, GPIO_FUNC_UART);
    uart_init(UART_INST, UART_BAUD_RATE);
#endif
}

void mod_UART::Tick() {
    
}