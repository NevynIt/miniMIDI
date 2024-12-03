#pragma once

//unused and untested

#include <array>

template <typename L>
class transition {
public:
    using S = typename L::State;
    using E = typename L::Event;
    using handler = S (L::*)(void *);

    S from;
    E event;
    S to;
    handler action = nullptr;
};

template<typename L>
class StateMachine : L {
public:
    using S = typename L::State;
    using E = typename L::Event;

    StateMachine(S startState) : currentState(startState) {}

    void next()
    {
        E event = nextEvent();
        for (auto &t : transitions) {
            void *res = match(currentState, event, t);
            if (res) {
                if (t.action)
                    currentState = (this->*t.action)(res);
                else
                    currentState = t.to;
                return;
            }
        }
    }

    S getState() const {
        return currentState;
    }

    void setState(S state) {
        currentState = state;
    }

    void reset() {
        currentState = getStartState();
    }

    int reach(S state, int maxSteps = 1000) {
        int steps = 0;
        while (currentState != state && steps < maxSteps) {
            next();
            steps++;
        }
        return steps;
    }

private:
    S currentState;
};

class SMLogic {
public:
    using State = uint8_t;
    using Event = uint8_t;
    using transition = transition<SMLogic>;

    State getStartState() {
        return 0;
    }

    void *match(State state, Event event, transition &t) {
        if (state == t.from && event == 0) {
            return (void *)&t;
        }
        return nullptr;
    }

    std::array<transition, 3> transitions = {
        transition{0, 0, 1, action1},
        transition{1, 1, 2, action2},
        transition{2, 2, 0, action1}
    };

    State action1(void *p) {
        transition *t = (transition *)p;
        return t->to;
    }

    State action2(void *p) {
        transition *t = (transition *)p;
        return t->to;
    }

    Event nextEvent() {
        return 0;
    }
};
