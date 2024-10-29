#include "mod_DSP.h"

void mod_DSP::Init() {
    // Initialize the DSP module
}

void mod_DSP::Tick() {
    // Perform any periodic DSP tasks if needed
}

void mod_DSP::GenerateSineWave(std::vector<float>& buffer, float frequency, float sampleRate) {
    buffer.resize(static_cast<size_t>(sampleRate / frequency));
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = std::sin(2.0f * M_PI * frequency * i / sampleRate);
    }
}

void mod_DSP::GenerateSquareWave(std::vector<float>& buffer, float frequency, float sampleRate) {
    buffer.resize(static_cast<size_t>(sampleRate / frequency));
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = (i < buffer.size() / 2) ? 1.0f : -1.0f;
    }
}

void mod_DSP::GenerateTriangleWave(std::vector<float>& buffer, float frequency, float sampleRate) {
    buffer.resize(static_cast<size_t>(sampleRate / frequency));
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = 2.0f * std::abs(2.0f * (i / static_cast<float>(buffer.size())) - 1.0f) - 1.0f;
    }
}

void mod_DSP::ApplyLowPassFilter(std::vector<float>& buffer, float cutoffFrequency, float sampleRate) {
    for (auto& sample : buffer) {
        sample = lowPassFilter(sample, cutoffFrequency, sampleRate);
    }
}

float mod_DSP::lowPassFilter(float input, float cutoffFrequency, float sampleRate) {
    float alpha = 2.0f * M_PI * cutoffFrequency / sampleRate;
    float output = alpha * input + (1.0f - alpha) * previousOutput;
    previousOutput = output;
    return output;
}