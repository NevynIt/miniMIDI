#ifndef MOD_UART_MIDI_H
#define MOD_UART_MIDI_H

#include "Module.h"
#include "Queue.h"
#include "Message.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

#define UART_ID uart0

class mod_UART_MIDI : public Module {
public:
    void Init() override;
    void Tick() override;

private:
    static constexpr uint UART_TX_PIN = 0;
    static constexpr uint UART_RX_PIN = 1;
    // static constexpr uart_inst* UART_ID = uart0;
    static constexpr uint BAUD_RATE = 31250; // Standard MIDI baud rate

    void ReadMIDI();
};

#endif // MOD_UART_MIDI_H