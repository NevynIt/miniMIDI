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

        tud_cdc_write(buffer, count);
        tud_cdc_write_flush();
    }
}