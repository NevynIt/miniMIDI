#ifndef MOD_SYNTH_H
#define MOD_SYNTH_H

#include "Module.h"
#include "pico_synth_ex.h"

class mod_Synth : public Module {
public:
    void Init() override;
    void Tick() override;

    void NoteOn(uint8_t key);
    void NoteOff(uint8_t key);
    void NoteToggle(uint8_t key);
    void AllNotesOff();
    void LoadPreset(Preset_t preset);
    void ControlMessage(control_message_t message);
    void SetParameter(synth_parameter_t parameter, int8_t value);
    int8_t GetOctaveShift();
    void PrintStatus();

private:
    // Add any private members if needed
};

#endif // MOD_SYNTH_H