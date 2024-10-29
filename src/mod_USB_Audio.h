#ifndef MOD_USB_AUDIO_H
#define MOD_USB_AUDIO_H

#include "Module.h"
#include "tusb.h"

class mod_USB_Audio : public Module {
public:
    void Init() override;
    void Tick() override;

    void SendAudioData(const uint8_t* data, size_t length);
};

#endif // MOD_USB_AUDIO_H