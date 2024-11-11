import math
import numpy as np
import wavetables as wt

import os
os.chdir(os.path.dirname(os.path.abspath(__file__)))

def GenerateSineWave(frequency, sample_rate, num_samples):
    audio_data = np.zeros(num_samples, dtype=np.float32)
    for i in range(num_samples):
        audio_data[i] = math.sin(2 * math.pi * frequency * i / sample_rate)
    return audio_data

def GenerateSquareWave(frequency, sample_rate, num_samples):
    audio_data = np.zeros(num_samples, dtype=np.float32)
    for i in range(num_samples):
        audio_data[i] = 1.0 if math.sin(2 * math.pi * frequency * i / sample_rate) > 0 else -1.0
    return audio_data

def GenerateSawtoothWave(frequency, sample_rate, num_samples):
    audio_data = np.zeros(num_samples, dtype=np.float32)
    for i in range(num_samples):
        audio_data[i] = 2 * (i % (sample_rate / frequency)) / (sample_rate / frequency) - 1
    return audio_data

def GenerateTriangleWave(frequency, sample_rate, num_samples):
    audio_data = np.zeros(num_samples, dtype=np.float32)
    for i in range(num_samples):
        audio_data[i] = 2 * abs(2 * (i % (sample_rate / frequency)) / (sample_rate / frequency) - 1) - 1
    return audio_data

def GenerateNoise(num_samples):
    audio_data = np.random.uniform(-1.0, 1.0, num_samples).astype(np.float32)
    return audio_data

def GenerateWaveforms(sample_rate, num_samples):
    audio_data = np.zeros(0, dtype=np.float32)
    audio_data = np.append(audio_data, GenerateSineWave(1, sample_rate, num_samples))
    audio_data = np.append(audio_data, GenerateSquareWave(1, sample_rate, num_samples))
    audio_data = np.append(audio_data, GenerateSawtoothWave(1, sample_rate, num_samples))
    audio_data = np.append(audio_data, GenerateTriangleWave(1, sample_rate, num_samples))
    audio_data = np.append(audio_data, GenerateNoise(num_samples))

    #convert audio_data to int16
    audio_data = (audio_data * 32767).astype(np.int16)
    
    waveforms = [
        (0, num_samples, "sin"),
        (num_samples, num_samples, "sqare"),
        (2 * num_samples, num_samples, "saw"),
        (3 * num_samples, num_samples, "triangle"),
        (4 * num_samples, num_samples, "noise"),
    ]
    return audio_data, waveforms

def main():
    sample_rate = 1024
    num_samples = 1024
    audio_data, waveforms = GenerateWaveforms(sample_rate, num_samples)
    wt.write_binary_file("base_tables.bin", audio_data, sample_rate, waveforms)

    with open("base_tables.bin", 'rb') as bin_file:
        bin_data = bin_file.read()

    wt.write_header_file("WT_BASE", bin_data, "WT_BASE", waveforms, sample_rate)
    wt.write_cpp_file("WT_BASE", bin_data, "WT_BASE")
    wt.write_wave_file("WT_BASE.wav", audio_data, sample_rate)

if __name__ == "__main__":
    main()