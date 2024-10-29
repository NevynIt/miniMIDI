#ifndef APP_H
#define APP_H

#include "Module.h"
#include "mod_UART_MIDI.h"
#include "mod_config.h"
#include <vector>
#include <memory>

class App : public Module {
public:
    static App& GetInstance();

    void Init() override;
    void Tick() override;

    void Init_c0();
    void Init_c1();
    void Tick_c0();
    void Tick_c1();

    mod_UART_MIDI uartMIDI;
    mod_config config;

private:
    App() = default; // Private constructor for singleton pattern
    static App appInstance;

    std::vector<Module*> modules_c0; // Modules to run on core 0
    std::vector<Module*> modules_c1; // Modules to run on core 1
};

#endif // APP_H