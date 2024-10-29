#ifndef MOD_USB_MIDI_H
#define MOD_USB_MIDI_H

#include "Module.h"
#include "tusb.h"

class mod_USB_MIDI : public Module {
public:
    void Init() override;
    void Tick() override;

    void SendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    void SendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
};

#endif // MOD_USB_MIDI_H