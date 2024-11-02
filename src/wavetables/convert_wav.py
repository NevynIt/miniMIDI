import wave
import numpy as np
import struct
import sys
import os

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
                offset, samples = map(int, line.split(','))
                waveforms.append((offset, samples))
    return waveforms

def write_binary_file(bin_filename, audio_data, sample_rate, waveforms):
    with open(bin_filename, 'wb') as bin_file:
        # Write the number of waveforms
        bin_file.write(struct.pack('<H', len(waveforms)))
        # Write the sample rate
        bin_file.write(struct.pack('<H', sample_rate))
        
        # Write the offset and length for each waveform
        for offset, samples in waveforms:
            bin_file.write(struct.pack('<HH', offset, samples))
        
        # Write the audio data
        bin_file.write(audio_data.tobytes())

def write_header_file(header_filename, bin_data, array_name):
    with open(header_filename, 'w') as header_file:
        header_file.write("#ifndef WAVETABLE_H\n")
        header_file.write("#define WAVETABLE_H\n\n")
        header_file.write(f"const uint16_t {array_name}[] = {{\n")
        for i in range(0, len(bin_data), 2):
            if i % 32 == 0 and i != 0:
                header_file.write("\n")
            value = struct.unpack('<H', bin_data[i:i+2])[0]
            header_file.write(f"    0x{value:04X},")
        header_file.write("\n};\n")
        header_file.write("#endif // WAVETABLE_H\n")

def write_cpp_file(cpp_filename, bin_data, array_name):
    with open(cpp_filename, 'w') as cpp_file:
        cpp_file.write('#include "wavetable.h"\n\n')
        cpp_file.write(f"const uint16_t {array_name}[] = {{\n")
        for i in range(0, len(bin_data), 2):
            if i % 32 == 0 and i != 0:
                cpp_file.write("\n")
            value = struct.unpack('<H', bin_data[i:i+2])[0]
            cpp_file.write(f"    0x{value:04X},")
        cpp_file.write("\n};\n")

def main(filename):
    wav_filename = f"{filename}.wav"
    txt_filename = f"{filename}.txt"
    bin_filename = f"{filename}.bin"
    header_filename = f"{filename}.h"
    cpp_filename = f"{filename}.cpp"
    array_name = f"WT_{filename}"

    if not os.path.exists(wav_filename):
        print(f"Error: {wav_filename} does not exist.")
        sys.exit(1)
    if not os.path.exists(txt_filename):
        print(f"Error: {txt_filename} does not exist.")
        sys.exit(1)

    audio_data, sample_rate = read_wav_file(wav_filename)
    waveforms = read_txt_file(txt_filename)
    write_binary_file(bin_filename, audio_data, sample_rate, waveforms)

    with open(bin_filename, 'rb') as bin_file:
        bin_data = bin_file.read()

    write_header_file(header_filename, bin_data, array_name)
    write_cpp_file(cpp_filename, bin_data, array_name)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python generate_wavetable.py <filename>")
        sys.exit(1)
    
    filename = sys.argv[1]
    main(filename)