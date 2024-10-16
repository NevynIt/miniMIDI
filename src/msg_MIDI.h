#ifndef MSG_MIDI_H
#define MSG_MIDI_H

#include "Message.h"
#include <cstddef>
#include <cstdint>

class msg_MIDI : public Message {
public:
    msg_MIDI(const uint8_t* data);
    ~msg_MIDI() override = default;

    void Process() override;
    size_t GetSize() const override;

    const uint8_t* GetData() const;
    static constexpr size_t MIDI_MESSAGE_SIZE = 4;

private:
    uint8_t data[MIDI_MESSAGE_SIZE];
};

#endif // MSG_MIDI_H