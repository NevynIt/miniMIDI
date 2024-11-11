#include "mod_DSP.h"
#include <cstdlib> // For rand()
#include <cstring> // For memset()
#include <cmath>   // For math functions
#include "hwConfig.h"
#include "stdio.h"
#include "App.h"
#include "midi_frequencies.h"
// #include "WT_BASE.h"
// #include "wave.h"
// #include "fpm/math.hpp"
#include "dsp/wave.h"

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
V0 @16 bits
Sine: 301.81 us
Square: 83.90 us
Sawtooth: 126.11 us
Triangle: 131.08 us
Noise: 58.67 us
Lookup table: 22.96 us
Lookup table interpolated: 69.93 us
V1 @16 bits
Lookup table new: ~9 us
V2 @32 bits
Noise: 51.67 us
Square: 13.55 us
Sawtooth: 31.71 us
Sine: 31.38 us
*/
template <typename WaveType>
void test_wave(WaveType wave, const char *name, sample_ptr buffer, size_t count, bool sdtest)
{
    uint32_t t0, t1;
    t0 = time_us_32();
    dsp::fillBuffer(buffer, wave, AUDIO_BUFFER_SAMPLES * count);
    t1 = time_us_32();
    printf("%s: %.2f us\n", name, (float)(t1 - t0) / count);
    if (sdtest)
        mMApp.sd.WriteFile(name, buffer, AUDIO_BUFFER_SAMPLES*1000*sizeof(sample_t));
}

void mod_DSP::Test()
{
    bool sdtest = false; 
    // sdtest = mMApp.sd.mounted; //Uncomment to write test files to SD card if present
    static bool test_done = false;
    if (test_done)
        return;

    test_done = true;
    printf("Starting DSP tests\n");

    if (sdtest)
        printf("SD Card test start\n");

    dsp::AngleType freq(440);
    sample_ptr buffer = new sample_t[AUDIO_BUFFER_SAMPLES * 1000];
    test_wave(dsp::noiseWave(), "Noise", buffer, 1000, sdtest);
    test_wave(dsp::squareWave(freq, dsp::SampleMax, 48000), "Square", buffer, 1000, sdtest);
    test_wave(dsp::sawtoothWave(freq, dsp::SampleMax, 48000), "Sawtooth", buffer, 1000, sdtest);
    test_wave(dsp::sinWave(freq, dsp::SampleMax, 48000), "Sine", buffer, 1000, sdtest);
    // test_wave(dsp::triangleWave(), "Triangle", buffer, 1000, sdtest);

    delete[] buffer;

    printf("DSP test done\n");
    if (sdtest)
        printf("SD Card test done\n");
}
