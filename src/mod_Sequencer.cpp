#include "mod_Sequencer.h"
#include "pico/stdlib.h"

void mod_Sequencer::Init() {
    // Initialize the sequencer
}

void mod_Sequencer::Tick() {
    uint32_t currentTime = to_ms_since_boot(get_absolute_time());

    // Fire all events that have a target time in the past
    while (!eventQueue.empty() && eventQueue.top().first <= currentTime) {
        Event event = eventQueue.top();
        eventQueue.pop();
        event.second(); // Call the event callback
    }
}

void mod_Sequencer::ScheduleEvent(uint32_t targetTime, EventCallback callback) {
    eventQueue.emplace(targetTime, callback);
}