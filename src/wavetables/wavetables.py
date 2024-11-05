import struct
import wave

def write_binary_file(bin_filename, audio_data, sample_rate, waveforms):
    with open(bin_filename, 'wb') as bin_file:
        # Write the number of waveforms
        bin_file.write(struct.pack('<h', len(waveforms)))  # Use 'h' instead of 'H'
        # Write the sample rate
        bin_file.write(struct.pack('<h', sample_rate))     # Use 'h' instead of 'H'
        
        # Write the offset and length for each waveform
        for offset, samples, name in waveforms:
            bin_file.write(struct.pack('<hh', offset + 1 + len(waveforms), samples))  # Use 'hh' instead of 'HH'
        
        # Write the audio data
        bin_file.write(audio_data.tobytes())

def write_header_file(filename, bin_data, array_name, waveforms, sample_rate):
    with open(filename + ".h", 'w') as header_file:
        header_file.write(f"#ifndef {array_name}_H\n")
        header_file.write(f"#define {array_name}_H\n\n")
        header_file.write("#include \"hwConfig.h\"\n\n")

        header_file.write(f"#define {array_name}_SAMPLE_RATE {sample_rate}\n")
        for i, (offset, samples, name) in enumerate(waveforms):
            header_file.write(f"#define {array_name}_{name} ({array_name} + {2 + len(waveforms)*2 + offset})\n")
            header_file.write(f"#define {array_name}_{name}_SIZE {samples}\n")
        header_file.write("\n")

        header_file.write(f"extern const sample_t {array_name}[{int(len(bin_data)/2)}];\n")  # Use 'int16_t'
        header_file.write(f"#endif // {array_name}_H\n")

def write_cpp_file(filename, bin_data, array_name):
    with open(filename + ".cpp", 'w') as cpp_file:
        cpp_file.write(f'#include "{filename}.h"\n\n')
        cpp_file.write(f"const sample_t {array_name}[{int(len(bin_data)/2)}] = {{\n")  # Use 'int16_t'
        for i in range(0, len(bin_data), 2):
            if i % 32 == 0 and i != 0:
                cpp_file.write("\n")
            value = struct.unpack('<h', bin_data[i:i+2])[0]  # Use '<h' instead of '<H'
            sign = '-' if value < 0 else '+'
            cpp_file.write(f"    {sign}0x{abs(value):04X},")
        cpp_file.write("\n};\n")

def write_wave_file(filename, audio_data, sample_rate):
    with wave.open(filename, 'wb') as wave_file:
        wave_file.setnchannels(1)
        wave_file.setsampwidth(2)
        wave_file.setframerate(sample_rate)
        wave_file.writeframes(audio_data.tobytes())