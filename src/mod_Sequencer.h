#ifndef MOD_SEQUENCER_H
#define MOD_SEQUENCER_H

#include "Module.h"
#include <functional>
#include <vector>
#include <queue>
#include <cstdint>
#include <utility>

class mod_Sequencer : public Module {
public:
    using EventCallback = std::function<void()>;

    void Init() override;
    void Tick() override;

    void ScheduleEvent(uint32_t targetTime, EventCallback callback);

private:
    using Event = std::pair<uint32_t, EventCallback>;
    struct CompareEvent {
        bool operator()(const Event& lhs, const Event& rhs) {
            return lhs.first > rhs.first;
        }
    };

    std::priority_queue<Event, std::vector<Event>, CompareEvent> eventQueue;
};

#endif // MOD_SEQUENCER_H