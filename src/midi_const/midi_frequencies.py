import math

import os
os.chdir(os.path.dirname(os.path.abspath(__file__)))

def generate_midi_frequencies(reference_frequency=440.0):
    header_content = """#ifndef MIDI_FREQUENCIES_H
#define MIDI_FREQUENCIES_H

#include "hwConfig.h"

extern const fp_int midi_frequencies[128];

#endif // MIDI_FREQUENCIES_H
"""

    cpp_content = """#include "midi_frequencies.h"
#include <cmath>

const fp_int midi_frequencies[128] = {
"""

    frequencies = []
    for i in range(128):
        frequency = reference_frequency * (2 ** ((i - 69) / 12.0))
        frequencies.append(f"    (fp_int)({frequency:.10f} * FP1),  // MIDI Note {i}")

    cpp_content += ",\n".join(frequencies)
    cpp_content += "\n};\n"

    with open("midi_frequencies.h", "w") as header_file:
        header_file.write(header_content)

    with open("midi_frequencies.cpp", "w") as cpp_file:
        cpp_file.write(cpp_content)

if __name__ == "__main__":
    generate_midi_frequencies()