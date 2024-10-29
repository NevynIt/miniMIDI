#ifndef MOD_DSP_H
#define MOD_DSP_H

#include "Module.h"
#include <vector>
#include <cmath>

class mod_DSP : public Module {
public:
    void Init() override;
    void Tick() override;

    void GenerateSineWave(std::vector<float>& buffer, float frequency, float sampleRate);
    void GenerateSquareWave(std::vector<float>& buffer, float frequency, float sampleRate);
    void GenerateTriangleWave(std::vector<float>& buffer, float frequency, float sampleRate);
    void ApplyLowPassFilter(std::vector<float>& buffer, float cutoffFrequency, float sampleRate);

private:
    float lowPassFilter(float input, float cutoffFrequency, float sampleRate);

    float previousInput = 0.0f;
    float previousOutput = 0.0f;
};

#endif // MOD_DSP_H