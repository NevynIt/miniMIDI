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

    static fp_int GenerateSineWave(sample_ptr buf, fp_int frequency = 440.0f * FP1, fp_int amplitude = FP1, fp_int phase = 0);
    static fp_int GenerateSquareWave(sample_ptr buf, fp_int frequency = 440.0f * FP1, fp_int amplitude = FP1, fp_int phase = 0);
    static fp_int GenerateTriangleWave(sample_ptr buf, fp_int frequency = 440.0f * FP1, fp_int amplitude = FP1, fp_int phase = 0);
    static fp_int GenerateSawtoothWave(sample_ptr buf, fp_int frequency = 440.0f * FP1, fp_int amplitude = FP1, fp_int phase = 0);
    static void GenerateNoise(sample_ptr buf, fp_int amplitude = FP1);
    static void MixBuffers(sample_ptr dst, sample_ptr src, fp_int dry, fp_int wet);
    static void ScaleBuffer(sample_ptr buf, sample_ptr gain);
    static void ClipBuffer(sample_ptr buf, fp_int threshold);
    static void AmplifyBuffer(sample_ptr buf, fp_int gain);
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
    static inline sample_t getSample(sample_ptr wavetable, uint16_t waveform, fp_int phase) {
        return WT_GetWaveform(wavetable, waveform)[(uint16_t)(phase * WT_GetWaveformSize(wavetable, waveform))];
    }
    static inline sample_t getSampleInterp(sample_ptr wavetable, uint16_t waveform, fp_int phase) {
        uint16_t size = WT_GetWaveformSize(wavetable, waveform);
        int index = phase * size;
        uint16_t index_int = static_cast<uint16_t>(index);
        int frac = index - index_int;

        sample_t sample1 = WT_GetWaveform(wavetable, waveform)[index_int];
        sample_t sample2 = WT_GetWaveform(wavetable, waveform)[(index_int + 1) % size];

        return static_cast<sample_t>(sample1 * (1.0f - frac) + sample2 * frac);
    }
    static fp_int GenerateWave(sample_ptr buf, sample_ptr wavetable, uint8_t waveform, fp_int frequency_ratio = FP1, fp_int phase = 0, bool interp = false);

    uint8_t getWritingSlot() const { return (currentSlot + 1) % AUDIO_BUFFER_SLOTS; }
    uint8_t getReadingSlot() const { return (currentSlot - 1 + AUDIO_BUFFER_SLOTS) % AUDIO_BUFFER_SLOTS; }
    uint8_t getDSPSlot() const { return currentSlot; }

    sample_t buffers[AUDIO_BUFFER_TRACKS][AUDIO_BUFFER_SLOTS][AUDIO_BUFFER_SAMPLES] = {0};
private:
    uint8_t currentSlot = 0;
    uint32_t time_reference = 0;
};

#endif // MOD_DSP_H