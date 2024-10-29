#include "mod_USB_MIDI.h"

void mod_USB_MIDI::Init() {
    // Initialization code for USB MIDI
}

void mod_USB_MIDI::Tick() {
    // Periodic tasks for USB MIDI
}

void mod_USB_MIDI::SendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t packet[4] = {0x09, 0x90 | channel, note, velocity};
    tud_midi_stream_write(0, packet, 4);
}

void mod_USB_MIDI::SendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t packet[4] = {0x08, 0x80 | channel, note, velocity};
    tud_midi_stream_write(0, packet, 4);
}