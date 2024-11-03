#include "mod_DSP.h"
#include <cstdlib> // For rand()
#include <cstring> // For memset()
#include <cmath>   // For math functions
#include "hwConfig.h"
#include "stdio.h"
#include "App.h"
#include "midi_frequencies.h"
#include "WT_BASE.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

fp_int sin_fixed(fp_int angle)
{
    angle = angle % (WT_BASE_sin_SIZE * FP1);
    return WT_BASE_sin[(angle * WT_BASE_sin_SIZE) / FP1] * FP1 / SAMPLE_MAX;
}

void mod_DSP::Init()
{
    // Initialize the DSP module
    time_reference = time_us_32();

    // fill the tracks with some data
    for (int t = 0; t < AUDIO_BUFFER_TRACKS; ++t)
    {
        fp_int phase = 0;
        for (int s = 0; s < AUDIO_BUFFER_SLOTS; ++s)
        {
            phase = GenerateSineWave(buffers[t][s], (t+1) * 110 * FP1, FP1, phase);
        }
    }

    app.display.println("DSP initialized");
    printf("DSP initialized\n");
}

void mod_DSP::Tick()
{
    // Perform any periodic DSP tasks if needed
    auto now = time_us_32();
    if (now - time_reference > AUDIO_BUFFER_MS * 1000)
    {
        // Perform DSP tasks
        time_reference += AUDIO_BUFFER_MS * 1000;
        currentSlot = (currentSlot + 1) % AUDIO_BUFFER_SLOTS;
    }
}

// Generators do not handle clipping correctly, they rely just on int overflow

fp_int mod_DSP::GenerateSineWave(sample_ptr buf, fp_int frequency, fp_int amplitude, fp_int phase)
{
    fp_int increment = (frequency / SAMPLE_RATE); // angle increment per sample
    fp_int angle = phase % FP1;
    sample_t range = (amplitude * SAMPLE_MAX) / FP1;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        fp_int s = sin_fixed(angle);
        buf[i] = SAMPLE_ZERO + range * s / FP1;
        angle = (angle + increment) % FP1;
    }
    return angle;
}

// __attribute__((optimize("O0")))
fp_int mod_DSP::GenerateSquareWave(sample_ptr buf, fp_int frequency, fp_int amplitude, fp_int phase)
{
    int period = (FP1 * SAMPLE_RATE / frequency); // period in samples
    int position = (phase * SAMPLE_RATE / frequency) % period;
    sample_t up = SAMPLE_ZERO + (amplitude * SAMPLE_MAX) / FP1;
    sample_t down = SAMPLE_ZERO - (amplitude * SAMPLE_MAX) / FP1;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (position < period / 2) ? up : down;
        position = (position + 1) % period;
    }
    return FP1 * position / period;
}

fp_int mod_DSP::GenerateSawtoothWave(sample_ptr buf, fp_int frequency, fp_int amplitude, fp_int phase)
{
    int period = (FP1 * SAMPLE_RATE / frequency); // period in samples
    int position = (phase * SAMPLE_RATE / frequency) % period;
    sample_t down = SAMPLE_ZERO - (amplitude * SAMPLE_MAX) / FP1;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        int pos = (position + i) % period;
        buf[i] = down + position * (amplitude * 2 * SAMPLE_MAX) / period / FP1;
    }
    return FP1 * position / period;
}

fp_int mod_DSP::GenerateTriangleWave(sample_ptr buf, fp_int frequency, fp_int amplitude, fp_int phase)
{
    int period = (FP1 * SAMPLE_RATE / frequency); // period in samples
    int position = (phase * SAMPLE_RATE / frequency) % period;
    sample_t up = SAMPLE_ZERO + (amplitude * SAMPLE_MAX) / FP1;
    sample_t down = SAMPLE_ZERO - (amplitude * SAMPLE_MAX) / FP1;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (position < period / 2)
                     ? down + position * (amplitude * SAMPLE_MAX) / period / FP1
                     : up - (position - period / 2) * (amplitude * SAMPLE_MAX) / period / FP1;
        position = (position + 1) % period;
    }
    return FP1 * position / period;
}

void mod_DSP::GenerateNoise(sample_ptr buf, fp_int amplitude)
{
    sample_t range = (amplitude * 2 * SAMPLE_MAX) / FP1;
    sample_t down = SAMPLE_ZERO - (amplitude * SAMPLE_MAX) / FP1;

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = down + (rand() % range);
    }
}

void mod_DSP::MixBuffers(sample_ptr dst, sample_ptr src, fp_int dry, fp_int wet)
{
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        dst[i] = dry * dst[i] / FP1 + wet * src[i] / FP1;
    }
}

void mod_DSP::ScaleBuffer(sample_ptr buf, sample_ptr gain)
{
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = ((((fp_int)gain[i]) - SAMPLE_ZERO) * FP1) / SAMPLE_MAX * (((fp_int)buf[i]) - SAMPLE_ZERO) / FP1 + SAMPLE_ZERO;
    }
}

void mod_DSP::ClipBuffer(sample_ptr buf, fp_int threshold)
{
    sample_t thresh = (threshold * SAMPLE_MAX) / FP1;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        if (buf[i] > SAMPLE_ZERO + thresh)
            buf[i] = SAMPLE_ZERO + thresh;
        else if (buf[i] < SAMPLE_ZERO - thresh)
            buf[i] = SAMPLE_ZERO - thresh;
    }
}

void mod_DSP::AmplifyBuffer(sample_ptr buf, fp_int gain)
{
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = gain * (((fp_int)buf[i]) - SAMPLE_ZERO) / FP1 + SAMPLE_ZERO;
    }
}

void mod_DSP::ClearBuffer(sample_ptr buf)
{
    memset(buf, SAMPLE_ZERO, AUDIO_BUFFER_SAMPLES * sizeof(sample_t));
}

fp_int mod_DSP::GenerateWave(sample_ptr buf, sample_ptr wavetable, uint8_t waveform, fp_int frequency_ratio, fp_int phase, bool interp)
{
    uint16_t size = WT_GetWaveformSize(wavetable, waveform);

    // index and increment are fixed-point numbers scaled by FP1
    uint32_t index = (uint32_t)(phase * size);
    uint32_t increment = ((uint64_t)frequency_ratio * size) / SAMPLE_RATE;

    sample_ptr waveform_data = WT_GetWaveform(wavetable, waveform);

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        uint16_t index_int = (uint16_t)(index / FP1);
        fp_int frac = (fp_int)(index % FP1);

        sample_t sample;
        if (interp) {
            sample_t sample1 = waveform_data[index_int];
            sample_t sample2 = waveform_data[(index_int + 1) % size];
            sample = sample1 + ((sample2 - sample1) * frac) / FP1;
        } else {
            sample = waveform_data[index_int];
        }

        buf[i] = sample;

        index += increment;
        if (index >= size * FP1) {
            index -= size * FP1;
        }
    }

    phase = (fp_int)(index / size);
    return phase;
}
