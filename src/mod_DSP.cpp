#include "mod_DSP.h"
#include <cstdlib>  // For rand()
#include <cstring>  // For memset()
#include <cmath>    // For math functions
#include "hwConfig.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int sin_fixed(int angle) {
    // Convert fixed-point angle to float radians
    float radians = (float)angle / FP1 * 2.0f * M_PI;
    // Use standard sinf function
    float sin_value = sinf(radians);
    // Convert result back to fixed-point
    return (int)(sin_value * FP1);
}

void mod_DSP::Init() {
    // Initialize the DSP module
    time_reference = time_us_32();
}

void mod_DSP::Tick() {
    // Perform any periodic DSP tasks if needed
    auto now = time_us_32();
    if (now - time_reference > AUDIO_BUFFER_MS * 1000) {
        // Perform DSP tasks
        time_reference += AUDIO_BUFFER_MS * 1000;
        currentSlot = (currentSlot + 1) % AUDIO_BUFFER_SLOTS;
    }
}

//Generators do not handle clipping correctly, they rely just on int overflow

fp_int mod_DSP::GenerateSineWave(sample_ptr buf, fp_int frequency, fp_int amplitude, fp_int phase) {
    fp_int increment = (frequency / SAMPLE_RATE); //angle increment per sample
    fp_int angle = phase & (FP1 - 1);
    sample_t half_range = amplitude * SAMPLE_MAX / 2 / FP1;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        buf[i] = SAMPLE_ZERO + half_range * sin_fixed(angle) / FP1;
        angle = (angle + increment) & (FP1 - 1);
    }
    return angle;
}

fp_int mod_DSP::GenerateSquareWave(sample_ptr buf, fp_int frequency, fp_int amplitude, fp_int phase) {
    int period = (FP1 * SAMPLE_RATE / frequency); //period in samples
    int position = (phase * SAMPLE_RATE / frequency) % period;
    sample_t up = SAMPLE_ZERO + amplitude * SAMPLE_MAX / 2 / FP1;
    sample_t down = SAMPLE_ZERO - amplitude * SAMPLE_MAX / 2 / FP1;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        buf[i] = (position < period / 2) ? up : down;
        position = (position + 1) % period;
    }
    return FP1 * position / period;
}

fp_int mod_DSP::GenerateSawtoothWave(sample_ptr buf, fp_int frequency, fp_int amplitude, fp_int phase) {
    int period = (FP1 * SAMPLE_RATE / frequency); //period in samples
    int position = (phase * SAMPLE_RATE / frequency) % period;
    sample_t down = SAMPLE_ZERO - amplitude * SAMPLE_MAX / 2 / FP1;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        int pos = (position + i) % period;
        buf[i] = down + position * amplitude * SAMPLE_MAX / period / FP1;
    }
    return FP1 * position / period;
}

fp_int mod_DSP::GenerateTriangleWave(sample_ptr buf, fp_int frequency, fp_int amplitude, fp_int phase) {
    int period = (FP1 * SAMPLE_RATE / frequency); //period in samples
    int position = (phase * SAMPLE_RATE / frequency) % period;
    sample_t up = SAMPLE_ZERO + amplitude * SAMPLE_MAX / 2 / FP1;
    sample_t down = SAMPLE_ZERO - amplitude * SAMPLE_MAX / 2 / FP1;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        buf[i] = (position < period / 2)
            ? down + position * amplitude * SAMPLE_MAX / period / 2 / FP1
            : up - (position - period / 2) * amplitude * SAMPLE_MAX / period / 2 / FP1;
        position = (position + 1) % period;
    }
    return FP1 * position / period;
}

void mod_DSP::GenerateNoise(sample_ptr buf, fp_int amplitude) {
    sample_t range = amplitude * SAMPLE_MAX / FP1;
    sample_t down = SAMPLE_ZERO - amplitude * SAMPLE_MAX / 2 / FP1;

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        buf[i] = down + (rand() % range);
    }
}

void mod_DSP::MixBuffers(sample_ptr dst, sample_ptr src, fp_int dry, fp_int wet) {
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        dst[i] = dry * dst[i] / FP1 + wet * src[i] / FP1;
    }
}

void mod_DSP::ScaleBuffer(sample_ptr buf, sample_ptr gain) {
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        buf[i] = (((fp_int)gain[i]) - SAMPLE_ZERO) * FP1 / (SAMPLE_MAX/2) * (((fp_int)buf[i]) - SAMPLE_ZERO) / FP1 + SAMPLE_ZERO;
    }
}

void mod_DSP::ClipBuffer(sample_ptr buf, fp_int threshold) {
    sample_t thresh = threshold * SAMPLE_MAX / FP1 / 2;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        if (buf[i] > SAMPLE_ZERO + thresh) buf[i] = SAMPLE_ZERO + thresh;
        else if (buf[i] < SAMPLE_ZERO - thresh) buf[i] = SAMPLE_ZERO - thresh;
    }
}

void mod_DSP::AmplifyBuffer(sample_ptr buf, fp_int gain) {
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        buf[i] = gain * (((fp_int)buf[i]) - SAMPLE_ZERO) / FP1 + SAMPLE_ZERO;
    }
}

void mod_DSP::ClearBuffer(sample_ptr buf) {
    memset(buf, SAMPLE_ZERO, AUDIO_BUFFER_SAMPLES * sizeof(sample_t));
}

fp_int mod_DSP::GenerateWave(sample_ptr buf, sample_ptr wavetable, uint8_t waveform, fp_int frequency_ratio, fp_int phase, bool interp) {
    return phase;
    
    // TODO Again using fixed point math

    // uint16_t size = WT_GetWaveformSize(wavetable, waveform);
    // float original_sample_rate = WT_GetSampleRate(wavetable);
    // float increment = (frequency_ratio * original_sample_rate) / SAMPLE_RATE;
    // float index = phase * size;

    // sample_ptr waveform_data = WT_GetWaveform(wavetable, waveform);

    // for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
    //     uint16_t index_int = static_cast<uint16_t>(index);
    //     float frac = index - index_int;

    //     sample_t sample;
    //     if (interp) {
    //         sample_t sample1 = waveform_data[index_int];
    //         sample_t sample2 = waveform_data[(index_int + 1) % size];
    //         sample = static_cast<sample_t>(sample1 * (1.0f - frac) + sample2 * frac);
    //     } else {
    //         sample = waveform_data[index_int];
    //     }

    //        buf[i] = sample;

    //     index += increment;
    //     if (index >= size) {
    //         index -= size;
    //     }
    // }

    // return index/size;
}

