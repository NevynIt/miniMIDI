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

    dsp::wave *ledWave = nullptr;

    void Toggle();
private:
    uint slice_num; // PWM slice number
};

#endif // MOD_BLINK_H
