#include "mod_DSP.h"
#include <cstdlib>  // For rand()
#include <cstring>  // For memset()
#include <cmath>    // For math functions
#include "hwConfig.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define fixed-point scaling factor
#define FIXED_POINT_SHIFT 16
#define FIXED_POINT_ONE (1 << FIXED_POINT_SHIFT)

int sin_fixed(int angle) {
    // Convert fixed-point angle to float radians
    float radians = (float)angle / FIXED_POINT_ONE * 2.0f * M_PI;
    // Use standard sinf function
    float sin_value = sinf(radians);
    // Convert result back to fixed-point
    return (int)(sin_value * FIXED_POINT_ONE);
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

float mod_DSP::GenerateSineWave(sample_ptr buf, float frequency, int amplitude, float phase, int channel) {
    int increment = static_cast<int>((frequency / SAMPLE_RATE) * FIXED_POINT_ONE);
    int angle = static_cast<int>(phase * FIXED_POINT_ONE) & (FIXED_POINT_ONE - 1);
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        int sample = (amplitude * sin_fixed(angle)) / FIXED_POINT_ONE;
        sample_t value = sample + (SAMPLE_MAX / 2);
        if (channel == -1) {
            for (int ch = 0; ch < SAMPLE_CHANNELS; ++ch)
                buf[i * SAMPLE_CHANNELS + ch] = value;
        } else {
            buf[i * SAMPLE_CHANNELS + channel] = value;
        }
        angle = (angle + increment) & (FIXED_POINT_ONE - 1);
    }
    return static_cast<float>(angle) / FIXED_POINT_ONE;
}

float mod_DSP::GenerateSquareWave(sample_ptr buf, float frequency, int amplitude, float phase, int channel) {
    int period = static_cast<int>(SAMPLE_RATE / frequency);
    int half_period = period / 2;
    int position = static_cast<int>(phase * SAMPLE_RATE / frequency) % period;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        int sample = (position < half_period) ? amplitude : -amplitude;
        sample_t value = sample + (SAMPLE_MAX / 2);
        if (channel == -1) {
            for (int ch = 0; ch < SAMPLE_CHANNELS; ++ch)
                buf[i * SAMPLE_CHANNELS + ch] = value;
        } else {
            buf[i * SAMPLE_CHANNELS + channel] = value;
        }
        position = (position + 1) % period;
    }
    return static_cast<float>(position) / SAMPLE_RATE * frequency;
}

float mod_DSP::GenerateTriangleWave(sample_ptr buf, float frequency, int amplitude, float phase, int channel) {
    int period = static_cast<int>(SAMPLE_RATE / frequency);
    int position = static_cast<int>(phase * SAMPLE_RATE / frequency) % period;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        int pos = (position + i) % period;
        int sample = (pos < period / 2)
            ? (2 * amplitude * pos) / (period / 2) - amplitude
            : (-2 * amplitude * (pos - period / 2)) / (period / 2) + amplitude;
        sample_t value = sample + (SAMPLE_MAX / 2);
        if (channel == -1) {
            for (int ch = 0; ch < SAMPLE_CHANNELS; ++ch)
                buf[(i * SAMPLE_CHANNELS) + ch] = value;
        } else {
            buf[(i * SAMPLE_CHANNELS) + channel] = value;
        }
    }
    return static_cast<float>((position + AUDIO_BUFFER_SAMPLES) % period) / SAMPLE_RATE * frequency;
}

float mod_DSP::GenerateSawtoothWave(sample_ptr buf, float frequency, int amplitude, float phase, int channel) {
    int period = static_cast<int>(SAMPLE_RATE / frequency);
    int position = static_cast<int>(phase * SAMPLE_RATE / frequency) % period;
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        int pos = (position + i) % period;
        int sample = ((2 * amplitude * pos) / period) - amplitude;
        sample_t value = sample + (SAMPLE_MAX / 2);
        if (channel == -1) {
            for (int ch = 0; ch < SAMPLE_CHANNELS; ++ch)
                buf[(i * SAMPLE_CHANNELS) + ch] = value;
        } else {
            buf[(i * SAMPLE_CHANNELS) + channel] = value;
        }
    }
    return static_cast<float>((position + AUDIO_BUFFER_SAMPLES) % period) / SAMPLE_RATE * frequency;
}

void mod_DSP::GenerateNoise(sample_ptr buf, int amplitude, int channel) {
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        int sample = (rand() % (2 * amplitude)) - amplitude;
        sample_t value = sample + (SAMPLE_MAX / 2);
        if (channel == -1) {
            for (int ch = 0; ch < SAMPLE_CHANNELS; ++ch)
                buf[i * SAMPLE_CHANNELS + ch] = value;
        } else {
            buf[i * SAMPLE_CHANNELS + channel] = value;
        }
    }
}

void mod_DSP::GenerateEnvelope(sample_ptr buf, float attack, float decay, float sustain, float release, int channel) {
    int total_samples = AUDIO_BUFFER_SAMPLES;
    int attack_samples = static_cast<int>(attack * SAMPLE_RATE);
    int decay_samples = static_cast<int>(decay * SAMPLE_RATE);
    int release_samples = static_cast<int>(release * SAMPLE_RATE);
    int sustain_samples = total_samples - attack_samples - decay_samples - release_samples;

    for (int i = 0; i < total_samples; ++i) {
        float envelope = 0.0f;
        if (i < attack_samples) {
            envelope = (float)i / attack_samples;
        } else if (i < attack_samples + decay_samples) {
            envelope = 1.0f - ((i - attack_samples) / (float)decay_samples) * (1.0f - sustain);
        } else if (i < attack_samples + decay_samples + sustain_samples) {
            envelope = sustain;
        } else {
            envelope = sustain * (1.0f - ((i - attack_samples - decay_samples - sustain_samples) / (float)release_samples));
        }
        sample_t value = static_cast<sample_t>(envelope * SAMPLE_MAX);
        if (channel == -1) {
            for (int ch = 0; ch < SAMPLE_CHANNELS; ++ch) {
                buf[i * SAMPLE_CHANNELS + ch] = value;
            }
        } else {
            buf[i * SAMPLE_CHANNELS + channel] = value;
        }
    }
}

void mod_DSP::MixBuffers(sample_ptr dst, sample_ptr src, float dry, float wet) {
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        for (int ch = 0; ch < SAMPLE_CHANNELS; ++ch) {
            int idx = i * SAMPLE_CHANNELS + ch;
            float dst_sample = dst[idx] / (float)SAMPLE_MAX;
            float src_sample = src[idx] / (float)SAMPLE_MAX;
            float mixed = dry * dst_sample + wet * src_sample;
            dst[idx] = static_cast<sample_t>(mixed * SAMPLE_MAX);
        }
    }
}

void mod_DSP::ApplyEnvelope(sample_ptr buf, sample_ptr envelope) {
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        for (int ch = 0; ch < SAMPLE_CHANNELS; ++ch) {
            int idx = i * SAMPLE_CHANNELS + ch;
            float env = envelope[idx] / (float)SAMPLE_MAX;
            buf[idx] = static_cast<sample_t>((buf[idx] / (float)SAMPLE_MAX) * env * SAMPLE_MAX);
        }
    }
}

void mod_DSP::ClipBuffer(sample_ptr buf, float threshold) {
    sample_t thresh = static_cast<sample_t>(threshold * SAMPLE_MAX);
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES * SAMPLE_CHANNELS; ++i) {
        if (buf[i] > thresh) buf[i] = thresh;
        else if (buf[i] < SAMPLE_MAX - thresh) buf[i] = SAMPLE_MAX - thresh;
    }
}

void mod_DSP::AmplifyBuffer(sample_ptr buf, float gain) {
    for (int i = 0; i < AUDIO_BUFFER_SAMPLES * SAMPLE_CHANNELS; ++i) {
        float sample = (buf[i] / (float)SAMPLE_MAX) * gain;
        if (sample > 1.0f) sample = 1.0f;
        else if (sample < 0.0f) sample = 0.0f;
        buf[i] = static_cast<sample_t>(sample * SAMPLE_MAX);
    }
}

void mod_DSP::ClearBuffer(sample_ptr buf) {
    memset(buf, 0, AUDIO_BUFFER_SAMPLES * SAMPLE_CHANNELS * sizeof(sample_t));
}

float mod_DSP::GenerateWave(sample_ptr buf, sample_ptr wavetable, uint8_t waveform, float frequency_ratio, float phase, int channel, bool interp) {
    uint16_t size = WT_GetWaveformSize(wavetable, waveform);
    float original_sample_rate = WT_GetSampleRate(wavetable);
    float increment = (frequency_ratio * original_sample_rate) / SAMPLE_RATE;
    float index = phase * size;

    sample_ptr waveform_data = WT_GetWaveform(wavetable, waveform);

    for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i) {
        uint16_t index_int = static_cast<uint16_t>(index);
        float frac = index - index_int;

        sample_t sample;
        if (interp) {
            sample_t sample1 = waveform_data[index_int];
            sample_t sample2 = waveform_data[(index_int + 1) % size];
            sample = static_cast<sample_t>(sample1 * (1.0f - frac) + sample2 * frac);
        } else {
            sample = waveform_data[index_int];
        }

        if (channel == -1) {
            for (int ch = 0; ch < SAMPLE_CHANNELS; ++ch) {
                buf[i * SAMPLE_CHANNELS + ch] = sample;
            }
        } else {
            buf[i * SAMPLE_CHANNELS + channel] = sample;
        }

        index += increment;
        if (index >= size) {
            index -= size;
        }
    }

    return index/size;
}

