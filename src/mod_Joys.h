#ifndef MOD_JOYS_H
#define MOD_JOYS_H

#include "Module.h"
#include "hardware/adc.h"
#include "hwConfig.h"

class mod_Joys : public Module {
public:
    void Init() override;
    void Tick() override;

    uint16_t GetX();
    uint16_t GetY();

private:
    uint16_t xValue;
    uint16_t yValue;
};

#endif // MOD_JOYS_H