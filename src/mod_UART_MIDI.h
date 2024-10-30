#ifndef MOD_UART_MIDI_H
#define MOD_UART_MIDI_H

#include "Module.h"
#include "Queue.h"
#include "Message.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hwConfig.h"

// #define UART_ID uart0

//Wait for either midi messages or a \n\n\n escape sequence, which would trigger interaction with a embedded-cli

class mod_UART_MIDI : public Module {
public:
    void Init() override;
    void Tick() override;

private:
    static constexpr uint BAUD_RATE = 31250; // Standard MIDI baud rate

    void ReadMIDI();
};

#endif // MOD_UART_MIDI_H