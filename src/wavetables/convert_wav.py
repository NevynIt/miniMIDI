import wave
import numpy as np
import sys
import os

import wavetables as wt

def read_wav_file(wav_filename):
    with wave.open(wav_filename, 'r') as wav_file:
        n_channels = wav_file.getnchannels()
        sample_width = wav_file.getsampwidth()
        sample_rate = wav_file.getframerate()
        n_frames = wav_file.getnframes()
        audio_data = wav_file.readframes(n_frames)
        
        if sample_width == 2:
            audio_data = np.frombuffer(audio_data, dtype=np.int16)
        else:
            raise ValueError("Only 16-bit WAV files are supported")
        
        if n_channels > 1:
            audio_data = audio_data.reshape(-1, n_channels)
            audio_data = audio_data.mean(axis=1).astype(np.int16)  # Convert to mono
        
        return audio_data, sample_rate

def read_txt_file(txt_filename):
    waveforms = []
    with open(txt_filename, 'r') as txt_file:
        for line in txt_file:
            line = line.split('#')[0].strip()  # Remove comments
            if line:
                offset, samples, name = map(int, line.split(','))
                waveforms.append((offset, samples, name))
    return waveforms

def main(filename):
    wav_filename = f"{filename}.wav"
    txt_filename = f"{filename}.txt"
    array_name = f"WT_{filename}"

    if not os.path.exists(wav_filename):
        print(f"Error: {wav_filename} does not exist.")
        sys.exit(1)
    if not os.path.exists(txt_filename):
        print(f"Error: {txt_filename} does not exist.")
        sys.exit(1)

    audio_data, sample_rate = read_wav_file(wav_filename)
    waveforms = read_txt_file(txt_filename)
    wt.write_binary_file(filename, audio_data, sample_rate, waveforms)

    with open(filename, 'rb') as bin_file:
        bin_data = bin_file.read()

    wt.write_header_file(array_name, bin_data, array_name, waveforms, sample_rate)
    wt.write_cpp_file(array_name, bin_data, array_name)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python generate_wavetable.py <filename>")
        sys.exit(1)
    
    filename = sys.argv[1]
    main(filename)