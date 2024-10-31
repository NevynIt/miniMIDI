#ifndef MOD_JOYS_H
#define MOD_JOYS_H

#include "Module.h"
#include "hardware/adc.h"
#include "hwConfig.h"

class mod_Joys : public Module {
public:
    void Init() override;
    void Tick() override;

    inline uint16_t GetX() const { return xValue; }
    inline uint16_t GetY() const { return yValue; }
    uint8_t button = 0;

private:
    uint16_t xValue;
    uint16_t yValue;
};

#endif // MOD_JOYS_H