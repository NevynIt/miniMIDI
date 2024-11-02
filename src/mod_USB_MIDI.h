#ifndef MOD_USB_MIDI_H
#define MOD_USB_MIDI_H

#include "Module.h"
#include "tusb.h"

class mod_USB_MIDI : public Module {
public:
    void Init() override;
    void Tick() override;
};

#endif // MOD_USB_MIDI_H