#ifndef MOD_BLINK_H
#define MOD_BLINK_H

#include "Module.h"
#include "hardware/gpio.h"
#include "hwConfig.h"

class mod_Blink : public Module {
public:
    void Init() override;
    void Tick() override;

private:
    bool ledState = false;
    uint32_t lastTickTime = 0;
};

#endif // MOD_BLINK_H
