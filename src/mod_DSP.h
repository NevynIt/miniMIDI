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
    void Test() override;

    static fp_int GenerateWave1Hz(sample_ptr buf, sample_cptr waveform, uint16_t size, const fp_int frequency = fp_int(440), const fp_int amplitude = fp_int(1), const fp_int phase = fp_int(0));
    static fp_int GenerateWave1HzInterp(sample_ptr buf, sample_cptr waveform, uint16_t size, const fp_int frequency = fp_int(440), const fp_int amplitude = fp_int(1), const fp_int phase = fp_int(0));
    static fp_int GenerateWave(sample_ptr buf, sample_cptr wavetable, uint8_t waveform_num, const fp_int frequency_ratio = fp_int(1), const fp_int amplitude = fp_int(1), const fp_int phase = fp_int(0));
    static fp_int GenerateWaveInterp(sample_ptr buf, sample_cptr wavetable, uint8_t waveform_num, const fp_int frequency_ratio = fp_int(1), const fp_int amplitude = fp_int(1), const fp_int phase = fp_int(0));
    static fp_int GenerateSineWave(sample_ptr buf, const fp_int frequency = fp_int(440), const fp_int amplitude = fp_int(1), const fp_int phase = fp_int(0));
    static fp_int GenerateSquareWave(sample_ptr buf, const fp_int frequency = fp_int(440), const fp_int amplitude = fp_int(1), const fp_int phase = fp_int(0));
    static fp_int GenerateTriangleWave(sample_ptr buf, const fp_int frequency = fp_int(440), const fp_int amplitude = fp_int(1), const fp_int phase = fp_int(0));
    static fp_int GenerateSawtoothWave(sample_ptr buf, const fp_int frequency = fp_int(440), const fp_int amplitude = fp_int(1), const fp_int phase = fp_int(0));
    static void GenerateNoise(sample_ptr buf, const fp_int amplitude = fp_int(1));
    static void MixBuffers(sample_ptr dst, sample_ptr src, const fp_int dry, const fp_int wet);
    static void ScaleBuffer(sample_ptr buf, sample_ptr gain);
    static void ClipBuffer(sample_ptr buf, const fp_int threshold);
    static void AmplifyBuffer(sample_ptr buf, const fp_int gain);
    static void ClearBuffer(sample_ptr buf);

    // waveform structure is:
    // 1 sample for the number of waveforms
    // 1 sample representing the original sampling frequency
    // 2 samples with offset and lenght for each waveform
    // the data samples
    static inline sample_cptr WT_GetWaveform(sample_cptr wavetable, uint8_t waveform) {
        return wavetable + WT_GetWaveformOffset(wavetable, waveform);
    }
    static inline uint16_t WT_GetWaveformOffset(sample_cptr wavetable, uint8_t waveform) {
        return wavetable[2 + (waveform * 2)];
    }
    static inline uint16_t WT_GetSampleRate(sample_cptr wavetable) {
        return wavetable[1];
    }
    static inline uint16_t WT_GetSize(sample_cptr wavetable) {
        return wavetable[0];
    }
    static inline uint16_t WT_GetWaveformSize(sample_cptr wavetable, uint8_t waveform) {
        return wavetable[2 + (waveform * 2) + 1];
    }
    static inline sample_t getSample(sample_cptr wavetable, uint16_t waveform, const fp_int phase) {
        return WT_GetWaveform(wavetable, waveform)[(uint16_t)(phase * WT_GetWaveformSize(wavetable, waveform))];
    }

    constexpr inline uint8_t getSlot(int delta = 0, uint64_t reference = 0) const {
        return (reference / AUDIO_BUFFER_US + delta) & AUDIO_BUFFER_SLOTS_MASK;
    }

    inline uint8_t getSlotRelative(int delta = 0, int32_t delta_time = 0) const {
        return getSlot(delta, time_us_64() + delta_time);
    }

    sample_ptr getBuffer(uint8_t track, int delta = 0, uint64_t reference = 0) {
        return buffers[track][getSlot(delta, reference)];
    }

    sample_t buffers[AUDIO_BUFFER_TRACKS][AUDIO_BUFFER_SLOTS][AUDIO_BUFFER_SAMPLES] = {0};
};

#endif // MOD_DSP_H