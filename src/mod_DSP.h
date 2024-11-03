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
        return wavetable + 2 + (waveform * 2);
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
    static inline sample_t getSampleInterp(sample_cptr wavetable, uint16_t waveform, const fp_int phase) {
        uint16_t size = WT_GetWaveformSize(wavetable, waveform);
        int index = (int)(phase * size);
        uint16_t index_int = static_cast<uint16_t>(index);
        int frac = index - index_int;

        sample_t sample1 = WT_GetWaveform(wavetable, waveform)[index_int];
        sample_t sample2 = WT_GetWaveform(wavetable, waveform)[(index_int + 1) % size];

        return static_cast<sample_t>(sample1 * (1.0f - frac) + sample2 * frac);
    }

    uint8_t getWritingSlot() const { return (currentSlot + 1) % AUDIO_BUFFER_SLOTS; }
    uint8_t getReadingSlot() const { return (currentSlot - 1 + AUDIO_BUFFER_SLOTS) % AUDIO_BUFFER_SLOTS; }
    uint8_t getDSPSlot() const { return currentSlot; }

    sample_ptr getWritingBuffer(uint8_t track) { return buffers[track][getWritingSlot()]; }
    sample_ptr getReadingBuffer(uint8_t track) { return buffers[track][getReadingSlot()]; }
    sample_ptr getDSPBuffer(uint8_t track) { return buffers[track][getDSPSlot()]; }

    sample_t buffers[AUDIO_BUFFER_TRACKS][AUDIO_BUFFER_SLOTS][AUDIO_BUFFER_SAMPLES] = {0};
private:
    uint8_t currentSlot = 0;
    uint32_t time_reference = 0;
};

#endif // MOD_DSP_H