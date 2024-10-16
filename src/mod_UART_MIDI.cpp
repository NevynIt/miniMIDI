#include "mod_UART_MIDI.h"
#include "msg_MIDI.h"

void mod_UART_MIDI::Init() {
    // Initialize UART
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

void mod_UART_MIDI::Tick() {
    // Read MIDI messages from UART and enqueue them
    ReadMIDI();
}

void mod_UART_MIDI::ReadMIDI() {
    //TODO: Implement reading MIDI messages from UART - this is broken
    //native MIDI messages are typically 3 bytes, need to add one at the beginning with the cable number
    //also, native MIDI might cache channel and message type

    Queue& inQueue = Queue::GetInQueue();

    while (uart_is_readable(UART_ID)) {
        uint8_t data[msg_MIDI::MIDI_MESSAGE_SIZE];
        for (size_t i = 0; i < msg_MIDI::MIDI_MESSAGE_SIZE; ++i) {
            if (uart_is_readable(UART_ID)) {
                data[i] = uart_getc(UART_ID);
            } else {
                // Handle incomplete MIDI message if necessary
                return;
            }
        }
        msg_MIDI* message = new (inQueue) msg_MIDI(data);
    }
}