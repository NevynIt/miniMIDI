#include "mod_Synth.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

void mod_Synth::Init() {
    // Set the system clock
    set_sys_clock_khz(120000, true); // Adjust the clock speed as needed

    // Initialize the synthesizer
#if USE_AUDIO_PWM
    PWMA_init(AUDIO_PIN_RIGHT, AUDIO_PIN_LEFT);
#elif USE_AUDIO_I2S
    sound_i2s_init(&sound_config);
    sound_i2s_playback_start();
    add_repeating_timer_ms(10, i2s_timer_callback, NULL, &i2s_timer);
#endif

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