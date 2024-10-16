#include "msg_MIDI.h"
#include <cstring>

msg_MIDI::msg_MIDI(const uint8_t* data) {
    std::memcpy(this->data, data, MIDI_MESSAGE_SIZE);
}

void msg_MIDI::Process() {
    // Implement the processing logic for the MIDI message
}

size_t msg_MIDI::GetSize() const {
    return sizeof(msg_MIDI);
}

const uint8_t* msg_MIDI::GetData() const {
    return data;
}