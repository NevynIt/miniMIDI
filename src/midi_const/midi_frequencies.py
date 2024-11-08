import math

import os
os.chdir(os.path.dirname(os.path.abspath(__file__)))

def generate_midi_frequencies(reference_frequency=440.0):
    header_content = """#ifndef MIDI_FREQUENCIES_H
#define MIDI_FREQUENCIES_H

extern const float midi_frequencies[128];

#endif // MIDI_FREQUENCIES_H
"""

    cpp_content = """#include "midi_frequencies.h"

const float midi_frequencies[128] = {
"""

    frequencies = []
    note_names = ['C', 'C#', 'D', 'D#', 'E', 'F',
                  'F#', 'G', 'G#', 'A', 'A#', 'B']
    for i in range(128):
        frequency = reference_frequency * (2 ** ((i - 69) / 12.0))
        note = note_names[i % 12]
        octave = (i // 12) - 1
        frequencies.append(f"    ({frequency:16.10f}),  // MIDI Note {i} ({note}{octave})")

    cpp_content += ",\n".join(frequencies)
    cpp_content += "\n};\n"

    with open("midi_frequencies.h", "w") as header_file:
        header_file.write(header_content)

    with open("midi_frequencies.cpp", "w") as cpp_file:
        cpp_file.write(cpp_content)

if __name__ == "__main__":
    generate_midi_frequencies()