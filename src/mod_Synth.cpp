#include "mod_Synth.h"
#include "pico/stdlib.h"

void mod_Synth::Init() {
    // Load a default preset
    control_message(PRESET_5);
}

void mod_Synth::Tick() {
    // Perform any periodic synthesizer tasks if needed
}

void mod_Synth::NoteOn(uint8_t key) {
    note_on(key);
}

void mod_Synth::NoteOff(uint8_t key) {
    note_off(key);
}

void mod_Synth::NoteToggle(uint8_t key) {
    note_toggle(key);
}

void mod_Synth::AllNotesOff() {
    all_notes_off();
}

void mod_Synth::LoadPreset(Preset_t preset) {
    load_preset(preset);
}

void mod_Synth::ControlMessage(control_message_t message) {
    control_message(message);
}

void mod_Synth::SetParameter(synth_parameter_t parameter, int8_t value) {
    set_parameter(parameter, value);
}

int8_t mod_Synth::GetOctaveShift() {
    return get_octave_shift();
}

void mod_Synth::PrintStatus() {
    print_status();
}