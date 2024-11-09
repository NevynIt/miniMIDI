#ifndef MOD_BLINK_H
#define MOD_BLINK_H

#include "Module.h"
#include "hardware/gpio.h"
#include "hwConfig.h"
#include "wave.h"
#include "wavetables/WT_BASE.h"

class mod_Blink : public Module {
public:
    void Init() override;
    void Tick() override;

    void Toggle();

    void setWaveform(sample_cptr waveform);

    float freq = 1;
    int level = 128;
private:
    bool ledState = false;
    uint32_t lastTickTime = 0;
    wave ledWave = wave(WT_BASE_sin); // Initialize with WT_BASE_sin
    uint slice_num; // PWM slice number
};

#endif // MOD_BLINK_H
