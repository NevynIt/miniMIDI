#ifndef mod_UART_H
#define mod_UART_H

#include "Module.h"
#include "Queue.h"
#include "Message.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hwConfig.h"
extern "C" {
#include "embedded_cli.h"
}

//Wait for either midi messages or a \n\n\n escape sequence, which would trigger interaction with a embedded-cli

class mod_UART : public Module {
public:
    void Init() override;
    void Tick() override;
    void out(const void *buffer, size_t count);

public:
    EmbeddedCli *cli;
};

#endif // mod_UART_H