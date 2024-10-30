#ifndef MOD_ENCODERS_H
#define MOD_ENCODERS_H

#include "Module.h"
#include "QuadratureDecoder.h"
#include <vector>
#include "hwConfig.h"

class mod_Encoders : public Module {
public:
    mod_Encoders() : decoder() {} // Ensure the constructor is defined
    void Init() override;
    void Tick() override;

    int32_t GetEncoderCount(int index);

private:
    QuadratureDecoder decoder;
    std::vector<int32_t> encoderCounts;
};

#endif // MOD_ENCODERS_H