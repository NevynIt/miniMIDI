#include "mod_USB_UART.h"
#include "App.h"

void mod_USB_UART::Init() {
    // Initialization code for USB UART
}

void mod_USB_UART::Tick() {
    // Periodic tasks for USB UART
    if (tud_cdc_available())
    {
        char buffer[64];
        size_t count = tud_cdc_read(buffer, sizeof(buffer));
        App::GetInstance().display.println(buffer);
        App::GetInstance().display.show();

        tud_cdc_write(buffer, count);
        tud_cdc_write_flush();
    }
}

void mod_USB_UART::Write(const char* data, size_t length) {
    tud_cdc_write(data, length);
    tud_cdc_write_flush();
}

size_t mod_USB_UART::Read(char* buffer, size_t bufferSize) {
    return tud_cdc_read(buffer, bufferSize);
}