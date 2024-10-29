#include "mod_USB_UART.h"

void mod_USB_UART::Init() {
    // Initialization code for USB UART
}

void mod_USB_UART::Tick() {
    // Periodic tasks for USB UART
}

void mod_USB_UART::Write(const char* data, size_t length) {
    tud_cdc_write(data, length);
    tud_cdc_write_flush();
}

size_t mod_USB_UART::Read(char* buffer, size_t bufferSize) {
    return tud_cdc_read(buffer, bufferSize);
}