#ifndef MOD_BLINK_H
#define MOD_BLINK_H

#include "Module.h"
#include "hardware/gpio.h"
#include "hwConfig.h"
#include "dsp/wave.h"

class mod_Blink : public Module {
public:
    void Init() override;
    void Tick() override;

    void Toggle();

    void setFreq(float freq);

    float freq = 1;
    int level = 128;
private:
    uint32_t lastTickTime = 0;
    dsp::sinWave ledWave;
    uint slice_num; // PWM slice number
};

#endif // MOD_BLINK_H
