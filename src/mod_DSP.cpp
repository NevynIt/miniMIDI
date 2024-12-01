#include "mod_DSP.h"
#include "hwConfig.h"
#include "stdio.h"
#include "App.h"
#include "midi_frequencies.h"
#include "dsp/wave.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"

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
V2 @16 bits
    Noise: 28.28 us
    Square: 13.14 us
    Sawtooth: 8.90 us
    Sine: 15.15 us

V3 @32 bits - my 64 bit multiplication
    Noise: 41.49 us
    Square: 7.65 us
    Sawtooth: 13.31 us
    Sine: 25.06 us
    harmonic: 204.52 us
    inharmonic: 232.83 us
V3 @32 bits - using native 64 bit multiplication
    Noise: 45.75 us
    Square: 7.63 us
    Sawtooth: 18.69 us
    Sine: 28.51 us
    harmonic: 198.48 us
    inharmonic: 262.88 us
V3 @32 bits - using native 64 bit multiplication and less bit shifting
    Noise: 39.96 us
    Square: 7.63 us
    Sawtooth: 18.68 us
    Sine: 23.72 us
    harmonic: 195.87 us
    inharmonic: 266.25 us
V3 @16 bits - my 64 bit multiplication
    Noise: 44.08 us
    Square: 8.23 us
    Sawtooth: 14.78 us
    Sine: 24.91 us
    harmonic: 225.47 us
    inharmonic: 271.67 us
    
V3 @16 bits - using native 32 bit multiplication
    Noise: 35.06 us
    Square: 8.23 us
    Sawtooth: 8.01 us
    Sine: 14.00 us
    harmonic: 78.83 us
    inharmonic: 94.74 us
V3 @16 bits - using native 32 bit multiplication and less bit shifting
    Noise: 28.99 us
    Square: 8.23 us
    Sawtooth: 8.01 us
    Sine: 9.71 us
    harmonic: 68.75 us
    inharmonic: 87.26 us

V4 @16 bits
    Noise: 26.89 us
    Square: 8.19 us
    Sawtooth: 8.29 us
    Sine: 8.54 us
    formant: 104.25 us

v4 @16 bits scriptable variant
    Noise: 45.18 us
    Square: 57.77 us
    Sawtooth: 26.62 us
    Sine: 29.37 us
    formant: 102.46 us
*/
template <typename WaveType>
void test_wave(WaveType wave, const char *name, sample_ptr buffer, size_t count, bool sdtest)
{
    printf("Testing %s: ", name);
    wave.inspect();
    const int rep = 30;
    uint32_t t = 0;
    uint32_t t0, t1;
    for (int i=0; i<rep; i++)
    {
        uint32_t irq_state = save_and_disable_interrupts();
        t0 = time_us_32();
        dsp::fillBuffer(buffer, wave, AUDIO_BUFFER_SAMPLES * count);
        t1 = time_us_32();
        restore_interrupts(irq_state);
        t += (t1 - t0);
    }
    printf("Time to generate 1ms of samples\n");
    printf("    %s: %.2f us\n", name, ((float)t/ (count*rep)));
    printf("\n");
    if (sdtest)
    {
        printf("Writing %s to SD card\n", name);
        mMApp.sd.WriteFile(name, buffer, AUDIO_BUFFER_SAMPLES*count*sizeof(sample_t));
    }
}

void mod_DSP::Test()
{
    static bool test_done = false;
    if (test_done)
        return;

    test_done = true;
    printf("Starting DSP tests\n");

    bool sdtest = false;
    if (sdtest && mMApp.sd.Mount())
        sdtest = true;

    if (sdtest)
        printf("SD Card test start\n");

    float freq(440);
    sample_ptr buffer = new sample_t[AUDIO_BUFFER_SAMPLES * 1000];
    dsp::PhaseType inc = dsp::inc_from_freq(freq, 48000);
    
    dsp::noise_wave noise;
    test_wave(noise, "Noise", buffer, 1000, sdtest);
    
    dsp::square_wave square;
    square.logic.S.increment = inc;
    test_wave(square, "Square", buffer, 1000, sdtest);

    dsp::sawtooth_wave sawtooth;
    sawtooth.logic.S.increment = inc;
    test_wave(sawtooth, "Sawtooth", buffer, 1000, sdtest);

    dsp::buffer_wave sine;
    sine.logic.S.increment = inc;
    test_wave(sine, "Sine", buffer, 1000, sdtest);

    dsp::formant_wave formant;
    formant.logic.S.increment = inc;
    float gains[5] = {1, 0.5, 0.25, 0.125, 0.0625};
    float ratios[5] = {1,2.5,5.4,12,15};
    dsp::normalize(gains);
    dsp::normalize(ratios);
    dsp::CoeffType gainsCoeff[5];
    dsp::CoeffType ratiosCoeff[5];
    for (int i = 0; i < 5; i++)
    {
        gainsCoeff[i] = fpm::from_float<dsp::CoeffDescr>(gains[i]);
        ratiosCoeff[i] = fpm::from_float<dsp::CoeffDescr>(ratios[i]);
    }
    formant.logic.I.gains = gainsCoeff;
    formant.logic.I.ratios = ratiosCoeff;
    formant.logic.S.count = 5;
    test_wave(formant, "formant", buffer, 1000, sdtest);

    delete[] buffer;

    printf("DSP test done\n");
    if (sdtest)
    {
        printf("SD Card test done\n");
        mMApp.sd.Unmount();
    }
}
