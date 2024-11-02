#ifndef MOD_DSP_H
#define MOD_DSP_H

#include "Module.h"
#include <vector>
#include <cmath>
#include "hwConfig.h"
#include "hardware/timer.h"

class mod_DSP : public Module {
public:
    void Init() override;
    void Tick() override;

    static float GenerateSineWave(sample_ptr buf, float frequency = 440.0f, int amplitude = SAMPLE_MAX / 2, float phase = 0.0f, int channel = -1);
    static float GenerateSquareWave(sample_ptr buf, float frequency = 440.0f, int amplitude = SAMPLE_MAX / 2, float phase = 0.0f, int channel = -1);
    static float GenerateTriangleWave(sample_ptr buf, float frequency = 440.0f, int amplitude = SAMPLE_MAX / 2, float phase = 0.0f, int channel = -1);
    static float GenerateSawtoothWave(sample_ptr buf, float frequency = 440.0f, int amplitude = SAMPLE_MAX / 2, float phase = 0.0f, int channel = -1);
    static void GenerateNoise(sample_ptr buf, int amplitude = SAMPLE_MAX / 2, int channel = -1);
    static void GenerateEnvelope(sample_ptr buf, float attack, float decay, float sustain, float release, int channel = -1);
    static void MixBuffers(sample_ptr dst, sample_ptr src, float dry, float wet);
    static void ApplyEnvelope(sample_ptr buf, sample_ptr envelope);
    static void ClipBuffer(sample_ptr buf, float threshold);
    static void AmplifyBuffer(sample_ptr buf, float gain);
    static void ClearBuffer(sample_ptr buf);


    // waveform structure is:
    // 1 sample for the number of waveforms
    // 1 sample representing the original sampling frequency
    // 2 samples with offset and lenght for each waveform
    // the data samples
    static inline sample_ptr WT_GetWaveform(sample_ptr wavetable, uint8_t waveform) {
        return wavetable + 2 + (waveform * 2);
    }
    static inline uint16_t WT_GetWaveformOffset(sample_ptr wavetable, uint8_t waveform) {
        return wavetable[2 + (waveform * 2)];
    }
    static inline uint16_t WT_GetSampleRate(sample_ptr wavetable) {
        return wavetable[1];
    }
    static inline uint16_t WT_GetSize(sample_ptr wavetable) {
        return wavetable[0];
    }
    static inline uint16_t WT_GetWaveformSize(sample_ptr wavetable, uint8_t waveform) {
        return wavetable[2 + (waveform * 2) + 1];
    }
    static inline sample_t getSample(sample_ptr wavetable, uint16_t waveform, float phase) {
        return WT_GetWaveform(wavetable, waveform)[(uint16_t)(phase * WT_GetWaveformSize(wavetable, waveform))];
    }
    static inline sample_t getSampleInterp(sample_ptr wavetable, uint16_t waveform, float phase) {
        uint16_t size = WT_GetWaveformSize(wavetable, waveform);
        float index = phase * size;
        uint16_t index_int = static_cast<uint16_t>(index);
        float frac = index - index_int;

        sample_t sample1 = WT_GetWaveform(wavetable, waveform)[index_int];
        sample_t sample2 = WT_GetWaveform(wavetable, waveform)[(index_int + 1) % size];

        return static_cast<sample_t>(sample1 * (1.0f - frac) + sample2 * frac);
    }
    static float GenerateWave(sample_ptr buf, sample_ptr wavetable, uint8_t waveform, float frequency_ratio = 1.0f, float phase = 0.0f, int channel = -1, bool interp = false);

    uint8_t getWritingSlot() const { return (currentSlot + 1) % AUDIO_BUFFER_SLOTS; }
    uint8_t getReadingSlot() const { return (currentSlot - 1 + AUDIO_BUFFER_SLOTS) % AUDIO_BUFFER_SLOTS; }
    uint8_t getDSPSlot() const { return currentSlot; }

    sample_t buffers[AUDIO_BUFFER_TRACKS][AUDIO_BUFFER_SLOTS][AUDIO_BUFFER_SAMPLES * SAMPLE_CHANNELS] = {0};
private:
    uint8_t currentSlot = 0;
    uint32_t time_reference = 0;
};

#endif // MOD_DSP_H