#ifndef MOD_ENCODERS_H
#define MOD_ENCODERS_H

#include "Module.h"
#include "hwConfig.h"

#ifdef ENCODERS_USE_PIO
#include "QuadratureDecoder.h"
#endif

class mod_Encoders : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    // Current quadrature count
    int32_t count[NUM_ENCODERS] = {0};
    uint8_t buttons = 0;
private:
#ifdef ENCODERS_USE_PIO
    QuadratureDecoder decoder;
#else
    void Decode();
#endif
};

#endif // MOD_ENCODERS_H