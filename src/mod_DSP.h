#ifndef MOD_DSP_H
#define MOD_DSP_H

#include "Module.h"
#include "hwConfig.h"
#include "hardware/timer.h"

class mod_DSP : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    constexpr inline uint8_t getSlot(int delta = 0, uint64_t reference = 0) const {
        return (reference / AUDIO_BUFFER_US + delta) & AUDIO_BUFFER_SLOTS_MASK;
    }

    inline uint8_t getSlotRelative(int delta = 0, int32_t delta_time = 0) const {
        return getSlot(delta, time_us_64() + delta_time);
    }

    sample_ptr getBuffer(uint8_t track, int delta = 0, uint64_t reference = 0) {
        return buffers[track][getSlot(delta, reference)];
    }

    sample_t buffers[AUDIO_BUFFER_TRACKS][AUDIO_BUFFER_SLOTS][AUDIO_BUFFER_SAMPLES] = {0};
};

#endif // MOD_DSP_H