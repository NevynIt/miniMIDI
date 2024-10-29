#ifndef MOD_JOYS_H
#define MOD_JOYS_H

#include "Module.h"
#include "hardware/adc.h"

class mod_Joys : public Module {
public:
    void Init() override;
    void Tick() override;

    uint16_t GetX();
    uint16_t GetY();

private:
    static const int JOY_X_PIN = 26; // ADC0
    static const int JOY_Y_PIN = 27; // ADC1
    uint16_t xValue;
    uint16_t yValue;
};

#endif // MOD_JOYS_H