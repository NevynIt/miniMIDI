#ifndef MOD_USB_AUDIO_H
#define MOD_USB_AUDIO_H

#include "Module.h"
#include "tusb.h"
#include "hwConfig.h"

class mod_USB_Audio : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    uint8_t track_out[CFG_TUD_AUDIO_FUNC_1_MAX_CHAN] = {DSP_TRACK_USB_OUT_LEFT, DSP_TRACK_USB_OUT_RIGHT};
    uint8_t track_in[CFG_TUD_AUDIO_FUNC_1_MAX_CHAN] = {DSP_TRACK_USB_IN_LEFT, DSP_TRACK_USB_IN_RIGHT};

    inline sample_ptr getOutBuffer(uint8_t channel) const;
    inline sample_ptr getInBuffer(uint8_t channel) const;

    int16_t buffer_in[AUDIO_BUFFER_SAMPLES*CFG_TUD_AUDIO_FUNC_1_MAX_CHAN] = {0}; // in from the USB host
    int16_t buffer_out[AUDIO_BUFFER_SAMPLES*CFG_TUD_AUDIO_FUNC_1_MAX_CHAN] = {0}; // out to the USB host
};

#endif // MOD_USB_AUDIO_H