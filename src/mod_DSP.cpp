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
#include "wave.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace fpm;

void mod_DSP::Init()
{
    mMApp.display.println("DSP initialized");
    printf("DSP initialized\n");
}

void mod_DSP::Tick()
{
    // Perform any periodic DSP tasks if needed
}

/*
Generators performance results on RP2040
Sine: 301.81 us
Square: 83.90 us
Sawtooth: 126.11 us
Triangle: 131.08 us
Noise: 58.67 us
Lookup table: 22.96 us
Lookup table interpolated: 69.93 us
*/
void mod_DSP::Test()
{
    bool sdtest = false; //Set to true to write test files to SD card
    sdtest = mMApp.sd.mounted; //Uncomment to write test files to SD card if present
    static bool test_done = false;
    if (test_done)
        return;

    test_done = true;
    printf("Starting DSP tests\n");

    if (sdtest)
        printf("SD Card test start\n");

    fp_int phase;
    sample_ptr buffer = new sample_t[AUDIO_BUFFER_SAMPLES * 1000];
    uint32_t t0, t1;

    // // Sine wave
    // phase = fp_int(0);
    // t0 = time_us_32();
    // for (int i = 0; i < 1000; ++i)
    // {
    //     phase = mMApp.dsp.GenerateSineWave(buffer + i * AUDIO_BUFFER_SAMPLES, fp_int(440), fp_int(1), phase);
    // }
    // t1 = time_us_32();
    // printf("Sine: %.2f us\n", (t1 - t0) / 1000.0f);
    // if (sdtest)
    //     mMApp.sd.WriteFile("sine.raw", std::string((char*)buffer, AUDIO_BUFFER_SAMPLES*1000*sizeof(sample_t)));

    // // Square wave
    // phase = fp_int(0);
    // t0 = time_us_32();
    // for (int i = 0; i < 1000; ++i)
    // {
    //     phase = mMApp.dsp.GenerateSquareWave(buffer + i * AUDIO_BUFFER_SAMPLES, fp_int(440), fp_int(1), phase);
    // }
    // t1 = time_us_32();
    // printf("Square: %.2f us\n", (t1 - t0) / 1000.0f);
    // if (sdtest)
    //     mMApp.sd.WriteFile("square.raw", std::string((char*)buffer, AUDIO_BUFFER_SAMPLES*1000*sizeof(sample_t)));

    // // Sawtooth wave
    // phase = fp_int(0);
    // t0 = time_us_32();
    // for (int i = 0; i < 1000; ++i)
    // {
    //     phase = mMApp.dsp.GenerateSawtoothWave(buffer + i * AUDIO_BUFFER_SAMPLES, fp_int(440), fp_int(1), phase);
    // }
    // t1 = time_us_32();
    // printf("Sawtooth: %.2f us\n", (t1 - t0) / 1000.0f);
    // if (sdtest)
    //     mMApp.sd.WriteFile("sawtooth.raw", buffer, AUDIO_BUFFER_SAMPLES * 1000 * sizeof(sample_t));

    // // Triangle wave
    // phase = fp_int(0);
    // t0 = time_us_32();
    // for (int i = 0; i < 1000; ++i)
    // {
    //     phase = mMApp.dsp.GenerateTriangleWave(buffer + i * AUDIO_BUFFER_SAMPLES, fp_int(440), fp_int(1), phase);
    // }
    // t1 = time_us_32();
    // printf("Triangle: %.2f us\n", (t1 - t0) / 1000.0f);
    // if (sdtest)
    //     mMApp.sd.WriteFile("triangle.raw", buffer, AUDIO_BUFFER_SAMPLES * 1000 * sizeof(sample_t));

    // // Noise
    // t0 = time_us_32();
    // for (int i = 0; i < 1000; ++i)
    // {
    //     mMApp.dsp.GenerateNoise(buffer + i * AUDIO_BUFFER_SAMPLES, fp_int(1));
    // }
    // t1 = time_us_32();
    // printf("Noise: %.2f us\n", (t1 - t0) / 1000.0f);
    // if (sdtest)
    //     mMApp.sd.WriteFile("noise.raw", std::string((char*)buffer, AUDIO_BUFFER_SAMPLES*1000*sizeof(sample_t)));

    // // Lookup table sin
    // phase = fp_int(0);
    // t0 = time_us_32();
    // for (int i = 0; i < 1000; ++i)
    // {
    //     phase = mMApp.dsp.GenerateWave(buffer + i * AUDIO_BUFFER_SAMPLES, WT_BASE, 0, fp_int(440), fp_int(1), phase);
    // }
    // t1 = time_us_32();
    // printf("Lookup table: %.2f us\n", (t1 - t0) / 1000.0f);
    // if (sdtest)
    //     mMApp.sd.WriteFile("lut.raw", buffer, AUDIO_BUFFER_SAMPLES * 1000 * sizeof(sample_t));

    // // Lookup table sin interp
    // phase = fp_int(0);
    // t0 = time_us_32();
    // for (int i = 0; i < 1000; ++i)
    // {
    //     phase = mMApp.dsp.GenerateWaveInterp(buffer + i * AUDIO_BUFFER_SAMPLES, WT_BASE, 0, fp_int(440), fp_int(1), phase);
    // }
    // t1 = time_us_32();
    // printf("Lookup table interpolated: %.2f us\n", (t1 - t0) / 1000.0f);
    // if (sdtest)
    //     mMApp.sd.WriteFile("lut_interp.raw", buffer, AUDIO_BUFFER_SAMPLES * 1000 * sizeof(sample_t));

    
    // Lookup table sin - NEW!
    t0 = time_us_32();
    auto w = wave(WT_BASE_sin, 440.0f);
    fill(buffer, AUDIO_BUFFER_SAMPLES * 1000, w);
    t1 = time_us_32();
    printf("Lookup table new: %.2f us\n", (t1 - t0) / 1000.0f);
    if (sdtest)
        mMApp.sd.WriteFile("wave.raw", buffer, AUDIO_BUFFER_SAMPLES * 1000 * sizeof(sample_t));

    
    // Lookup table sin freq_mod - NEW!
    t0 = time_us_32();
    auto w1 = wave(WT_BASE_sin, 440.0f);
    auto w2 = wave(WT_BASE_sin, 11.0f);
    auto w3 = wave_mod_FM(w1, w2, 20.0f);
    fill(buffer, AUDIO_BUFFER_SAMPLES * 1000, w3);
    t1 = time_us_32();
    printf("lookup table FM modulation: %.2f us\n", (t1 - t0) / 1000.0f);
    if (sdtest)
        mMApp.sd.WriteFile("waveFM.raw", buffer, AUDIO_BUFFER_SAMPLES * 1000 * sizeof(sample_t));


    delete[] buffer;

    printf("DSP test done\n");
    if (sdtest)
        printf("SD Card test done\n");
}

// Generators do not handle clipping correctly, they rely just on int overflow

// //new implementation, super optimized, I am curious about the performance! OBE by wave.h
// sample_t mod_DSP::GenerateWave1Hz(sample_ptr buf, sample_cptr waveform, const fp_int frequency, const sample_t amplitude, const sample_t phase)
// {
//     const fp_int increment = frequency * fp_int(WAVEFORM_SIZE / SAMPLE_RATE); // index increment per sample
//     fp_int index = fp_int::from_raw_value(phase) << WAVEFORM_SIZE_BITS; // convert phase to index

//     for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
//     {
//         buf[i] = (sample_t)((int32_t(amplitude) * waveform[(int)(index)]) >> 15);
//         index += increment;
//         index &= WAVEFORM_SIZE_FP_MASK;
//     }
//     return static_cast<int16_t>((index >> WAVEFORM_SIZE_BITS).raw_value());
// }

// fp_int mod_DSP::GenerateWave1HzInterp(sample_ptr buf, sample_cptr waveform, uint16_t size, const fp_int frequency, const fp_int amplitude, const fp_int phase)
// {
//     fp_int increment = frequency / SAMPLE_RATE; // angle increment per sample
//     fp_int angle = phase;

//     for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
//     {
//         int index = (int)(angle * size);
//         int next_index = (index + 1) % size;
//         fp_int frac = mod1(angle * size);
//         buf[i] = (sample_t)(amplitude * ((1 - frac) * waveform[index] + frac * waveform[next_index]) + SAMPLE_ZERO);
//         angle = mod1(angle + increment);
//     }
//     return angle;
// }

// __attribute__((optimize("O0"))) // function attribute to disable optimization
fp_int mod_DSP::GenerateWave(sample_ptr buf, sample_cptr wavetable, uint8_t waveform_num, const fp_int frequency_ratio, const fp_int amplitude, const fp_int phase)
{
    const uint16_t size = WT_GetWaveformSize(wavetable, waveform_num);
    const sample_cptr waveform = WT_GetWaveform(wavetable, waveform_num);
    const uint16_t sample_rate = WT_GetSampleRate(wavetable);
    const fp_int period = fp_int(float(SAMPLE_RATE) / size * sample_rate / float(frequency_ratio)); // period in samples
    const fp_int increment = size / period; // index increment per sample
    fp_int index = phase * size;

    int i;
    for (i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (sample_t)(amplitude * waveform[(int)(index)]);
        index += increment;
        if (index >= size)
            index -= size;
    }
    return (index / size).mod1();
}

// __attribute__((optimize("O0"))) // function attribute to disable optimization
fp_int mod_DSP::GenerateWaveInterp(sample_ptr buf, sample_cptr wavetable, uint8_t waveform_num, const fp_int frequency_ratio, const fp_int amplitude, const fp_int phase)
{
    const uint16_t size = WT_GetWaveformSize(wavetable, waveform_num);
    const sample_cptr waveform = WT_GetWaveform(wavetable, waveform_num);
    const uint16_t sample_rate = WT_GetSampleRate(wavetable);
    const fp_int period = fp_int(float(SAMPLE_RATE) / size * sample_rate / float(frequency_ratio)); // period in samples
    const fp_int increment = size/period; // index increment per sample
    fp_int index = phase * size;

    int i;
    for (i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        fp_int frac = index.mod1();
        int index_plus_0 = (int)(index);
        int index_plus_1 = (index_plus_0 + 1);
        if (index_plus_1 >= size)
            index_plus_1 -= size;
        buf[i] = (sample_t)(amplitude * ((1-frac) * waveform[index_plus_0] + frac * waveform[index_plus_1]));
        index += increment;
        if (index >= size)
            index -= size;
    }
    return (index / size).mod1();
}

// __attribute__((optimize("O0"))) // function attribute to disable optimization
fp_int mod_DSP::GenerateSineWave(sample_ptr buf, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    // return GenerateWave1Hz(buf, WT_BASE_sin, WT_BASE_sin_SIZE, frequency, amplitude, phase);

    fp_int increment = frequency / SAMPLE_RATE; // angle increment per sample
    fp_int angle = phase;
    const fp_int range = amplitude * SAMPLE_MAX;

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (sample_t)(range * sin(angle * TWO_PI) + SAMPLE_ZERO);
        angle = (angle + increment).mod1();
    }
    return angle;
}

// __attribute__((optimize("O0"))) // function attribute to disable optimization
fp_int mod_DSP::GenerateSquareWave(sample_ptr buf, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    fp_int period = SAMPLE_RATE / frequency; // period in samples
    const sample_t up = (sample_t)(SAMPLE_ZERO + (amplitude * SAMPLE_MAX));
    const sample_t down = (sample_t)(SAMPLE_ZERO - (amplitude * SAMPLE_MAX));
    int i;
    for (i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = ((phase + i / period).mod1() < fp_int(0.5)) ? up : down;
    }
    return (phase + i / period).mod1();
}

// __attribute__((optimize("O0"))) // function attribute to disable optimization
fp_int mod_DSP::GenerateSawtoothWave(sample_ptr buf, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    fp_int period = SAMPLE_RATE / frequency; // period in samples
    // const sample_t down = (sample_t)(SAMPLE_ZERO - (amplitude * SAMPLE_MAX));
    int i;
    for (i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        // buf[i] = down + (sample_t)(mod1(phase + i / period) * (amplitude * 2 * SAMPLE_MAX) / period);
        buf[i] = (sample_t)(((amplitude * ((phase + i / period).mod1()) * 2 - 1) * SAMPLE_MAX) + SAMPLE_ZERO);
    }
    return (phase + i / period).mod1();
}

// __attribute__((optimize("O0"))) // function attribute to disable optimization
fp_int mod_DSP::GenerateTriangleWave(sample_ptr buf, const fp_int frequency, const fp_int amplitude, const fp_int phase)
{
    fp_int period = SAMPLE_RATE / frequency; // period in samples
    int i;
    for (i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = (sample_t)((fp_int(0.5f) - 2 * abs(fp_int(0.5f) - (phase + i / period + fp_int(0.25f)).mod1())) * 2 * amplitude * SAMPLE_MAX + SAMPLE_ZERO);
    }
    return (phase + i / period).mod1();
}

// __attribute__((optimize("O0"))) // function attribute to disable optimization
void mod_DSP::GenerateNoise(sample_ptr buf, const fp_int amplitude)
{
    const sample_t range = (sample_t)(amplitude * SAMPLE_MAX);

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        buf[i] = ((rand() % 2) ? 1 : -1) * (rand() % range);
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