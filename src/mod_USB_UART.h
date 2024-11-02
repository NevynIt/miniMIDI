#ifndef MOD_USB_UART_H
#define MOD_USB_UART_H

#include "Module.h"
#include "tusb.h"

//use embedded-cli to interact with the device

class mod_USB_UART : public Module {
public:
    void Init() override;
    void Tick() override;

};

#endif // MOD_USB_UART_H