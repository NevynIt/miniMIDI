#include "mod_DSP.h"
#include <cstdlib> // For rand()
#include <cstring> // For memset()
#include <cmath>   // For math functions
#include "hwConfig.h"
#include "stdio.h"
#include "App.h"
#include "midi_frequencies.h"
#include "WT_BASE.h"
#include "fpm/math.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace fpm;

void mod_DSP::Init()
{
    app.display.println("DSP initialized");
    printf("DSP initialized\n");
}

void mod_DSP::Tick()
{
    // Perform any periodic DSP tasks if needed
}

/*
//Fixed point performance
GenerateWave1Hz: 28.17 us
GenerateWave1HzInterp: 77.48 us
GenerateSineWave: 278.97 us
GenerateSquareWave: 95.47 us
GenerateSawtoothWave: 225.88 us
GenerateTriangleWave: 144.86 us
GenerateNoise: 37.24 us

//Float performance (same code, just changing the type)
GenerateWave1Hz: 227.91 us
GenerateWave1HzInterp: 436.55 us
GenerateSineWave: 741.62 us
GenerateSquareWave: 109.59 us
GenerateSawtoothWave: 202.01 us
GenerateTriangleWave: 270.84 us
GenerateNoise: 36.94 us
*/
void mod_DSP::Test() 
{
    static bool test_done = false;
    if (test_done)
        return;

    test_done = true;
    const auto reps = 1000;
    printf("Starting DSP tests\n");

    //Measure the speed of each of the generator functions, filling a buffer 100 times with a 440hz wave
    sample_t buffer[AUDIO_BUFFER_SAMPLES];
    uint32_t t0, t1;

    t0 = time_us_32();
    for (int i = 0; i < reps; ++i)
        GenerateWave1Hz(buffer, WT_BASE_sin, WT_BASE_sin_SIZE, fp_int(440), fp_int(1), fp_int(0));
    t1 = time_us_32();
    printf("GenerateWave1Hz: %.2f us\n", (t1 - t0) / (float)reps);

    t0 = time_us_32();
    for (int i = 0; i < reps; ++i)
        GenerateWave1HzInterp(buffer, WT_BASE_sin, WT_BASE_sin_SIZE, fp_int(440), fp_int(1), fp_int(0));
    t1 = time_us_32();
    printf("GenerateWave1HzInterp: %.2f us\n", (t1 - t0) / (float)reps);

    t0 = time_us_32();
    for (int i = 0; i < reps; ++i)
        GenerateSineWave(buffer, fp_int(440), fp_int(1), fp_int(0));
    t1 = time_us_32();
    printf("GenerateSineWave: %.2f us\n", (t1 - t0) / (float)reps);

    t0 = time_us_32();
    for (int i = 0; i < reps; ++i)
        GenerateSquareWave(buffer, fp_int(440), fp_int(1), fp_int(0));
    t1 = time_us_32();
    printf("GenerateSquareWave: %.2f us\n", (t1 - t0) / (float)reps);

    t0 = time_us_32();
    for (int i = 0; i < reps; ++i)
        GenerateSawtoothWave(buffer, fp_int(440), fp_int(1), fp_int(0));
    t1 = time_us_32();
    printf("GenerateSawtoothWave: %.2f us\n", (t1 - t0) / (float)reps);

    t0 = time_us_32();
    for (int i = 0; i < reps; ++i)
        GenerateTriangleWave(buffer, fp_int(440), fp_int(1), fp_int(0));
    t1 = time_us_32();
    printf("GenerateTriangleWave: %.2f us\n", (t1 - t0) / (float)reps);

    t0 = time_us_32();
    for (int i = 0; i < reps; ++i)
        GenerateNoise(buffer, fp_int(1));
    t1 = time_us_32();
    printf("GenerateNoise: %.2f us\n", (t1 - t0) / (float)reps);
}

// Generators do not handle clipping correctly, they rely just on int overflow

fp_int mod_DSP::GenerateWave1Hz(sample_ptr buf, sample_cptr waveform, uint16_t size, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    fp_int increment = frequency / SAMPLE_RATE; // angle increment per sample
    fp_int angle = phase;

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (sample_t)(amplitude * waveform[(int)(angle * size)] + SAMPLE_ZERO);
        angle = mod1(angle + increment);
    }
    return angle;
}

fp_int mod_DSP::GenerateWave1HzInterp(sample_ptr buf, sample_cptr waveform, uint16_t size, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    fp_int increment = frequency / SAMPLE_RATE; // angle increment per sample
    fp_int angle = phase;

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        int index = (int)(angle * size);
        int next_index = (index + 1) % size;
        fp_int frac = mod1(angle * size);
        buf[i] = (sample_t)(amplitude * ((1 - frac) * waveform[index] + frac * waveform[next_index]) + SAMPLE_ZERO);
        angle = mod1(angle + increment);
    }
    return angle;
}

fp_int mod_DSP::GenerateWave(sample_ptr buf, sample_cptr wavetable, uint8_t waveform_num, const fp_int frequency_ratio, const fp_int amplitude, const fp_int phase)
{
    uint16_t size = WT_GetWaveformSize(wavetable, waveform_num);
    sample_cptr waveform = WT_GetWaveform(wavetable, waveform_num);
    uint16_t sample_rate = WT_GetSampleRate(wavetable);


    fp_int increment = frequency_ratio * sample_rate / SAMPLE_RATE / size; // source increment per destination sample
    fp_int angle = phase;

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (sample_t)((amplitude * waveform[(int)(angle * size)]) + SAMPLE_ZERO);
        angle = mod1(angle + increment);
    }
    return mod1(angle/size);
}


fp_int mod_DSP::GenerateWaveInterp(sample_ptr buf, sample_cptr wavetable, uint8_t waveform_num, const fp_int frequency_ratio, const fp_int amplitude, const fp_int phase)
{
    uint16_t size = WT_GetWaveformSize(wavetable, waveform_num);
    sample_cptr waveform = WT_GetWaveform(wavetable, waveform_num);
    uint16_t sample_rate = WT_GetSampleRate(wavetable);


    fp_int increment = frequency_ratio * sample_rate / SAMPLE_RATE / size; // source increment per destination sample
    fp_int angle = phase;

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        int index = (int)(angle * size);
        int next_index = (index + 1) % size;
        fp_int frac = mod1(angle * size);
        buf[i] = (sample_t)(amplitude * ((1 - frac) * waveform[index] + frac * waveform[next_index]) + SAMPLE_ZERO);
        angle = mod1(angle + increment);
    }
    return mod1(angle / size);
}

fp_int mod_DSP::GenerateSineWave(sample_ptr buf, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    // return GenerateWave1Hz(buf, WT_BASE_sin, WT_BASE_sin_SIZE, frequency, amplitude, phase);

    fp_int increment = frequency / SAMPLE_RATE; // angle increment per sample
    fp_int angle = phase;
    const fp_int range = amplitude * SAMPLE_MAX;

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (sample_t)(range * sin(angle) + SAMPLE_ZERO);
        angle = mod1(angle + increment);
    }
    return angle;
}

fp_int mod_DSP::GenerateSquareWave(sample_ptr buf, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    fp_int period = SAMPLE_RATE / frequency; // period in samples
    const sample_t up = (sample_t)(SAMPLE_ZERO + (amplitude * SAMPLE_MAX));
    const sample_t down = (sample_t)(SAMPLE_ZERO - (amplitude * SAMPLE_MAX));
    int i;
    for (i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (mod1(i / period) < fp_int(0.5)) ? up : down;
    }
    return mod1(i / period);
}

fp_int mod_DSP::GenerateSawtoothWave(sample_ptr buf, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    fp_int period = SAMPLE_RATE / frequency; // period in samples
    const sample_t down = (sample_t)(SAMPLE_ZERO - (amplitude * SAMPLE_MAX));
    int i;
    for (i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = down + (sample_t)(mod1(i / period) * (amplitude * 2 * SAMPLE_MAX) / period);
    }
    return mod1(i / period);
}

fp_int mod_DSP::GenerateTriangleWave(sample_ptr buf, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    fp_int period = SAMPLE_RATE / frequency; // period in samples
    const sample_t up = (sample_t)(SAMPLE_ZERO + (amplitude * SAMPLE_MAX));
    const sample_t down = (sample_t)(SAMPLE_ZERO - (amplitude * SAMPLE_MAX));
    int i;
    for (i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        auto pos = mod1(i / period);
        buf[i] = (sample_t)((pos > fp_int(0.5))
                     ? down + pos * (amplitude * 2 * SAMPLE_MAX)
                     : up - (pos - fp_int(0.5)) * (amplitude * 2 * SAMPLE_MAX));
    }
    return mod1(i / period);
}

void mod_DSP::GenerateNoise(sample_ptr buf, const fp_int amplitude)
{
    const sample_t range = (sample_t)(amplitude * 2 * SAMPLE_MAX);
    const sample_t down = (sample_t)(SAMPLE_ZERO - (amplitude * SAMPLE_MAX));

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = down + (rand() % range);
    }
}

void mod_DSP::MixBuffers(sample_ptr dst, sample_ptr src, const fp_int dry, const fp_int wet)
{
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        dst[i] = (sample_t)(dry * dst[i] + wet * src[i]);
    }
}

void mod_DSP::ScaleBuffer(sample_ptr buf, sample_ptr gain)
{
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (sample_t)((fp_int(gain[i]) - SAMPLE_ZERO) / SAMPLE_MAX * ((fp_int(buf[i]) - SAMPLE_ZERO) + SAMPLE_ZERO));
    }
}

void mod_DSP::ClipBuffer(sample_ptr buf, const fp_int threshold)
{
    sample_t thresh = (sample_t)(threshold * SAMPLE_MAX);
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        if (buf[i] > SAMPLE_ZERO + thresh)
            buf[i] = SAMPLE_ZERO + thresh;
        else if (buf[i] < SAMPLE_ZERO - thresh)
            buf[i] = SAMPLE_ZERO - thresh;
    }
}

void mod_DSP::AmplifyBuffer(sample_ptr buf, const fp_int gain)
{
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (sample_t)(gain * ((fp_int(buf[i]) - SAMPLE_ZERO) + SAMPLE_ZERO));
    }
}

void mod_DSP::ClearBuffer(sample_ptr buf)
{
    memset(buf, SAMPLE_ZERO, AUDIO_BUFFER_SAMPLES * sizeof(sample_t));
}