#include "App.h"

//  __attribute__((optimize("O0"))) // function attribute to disable optimization

// Define the static instance of App
App App::appInstance;

void App::Init() {
    // Empty, as Init_c0 and Init_c1 will handle initialization
}

void App::Tick() {
    // Empty, as Tick_c0 and Tick_c1 will handle ticking
}

void App::Init_c0() {
    std::vector<Module*> &mods =  modules_c0;

    // non time critical modules on core 0
    mods.push_back(&blink); // basic visual feedback of operation

    // User interface modules
    mods.push_back(&uart); //Supports STDIO for debugging if enabled on CMakelists.txt
    mods.push_back(&config); //Dummy module for now

    mods.push_back(&dsp);
    mods.push_back(&sd);
    
    // USB modules
    mods.push_back(&usb);
    mods.push_back(&usbUart); //Echoes data back to the sender
    mods.push_back(&usbMidi); //Empty module for now
    
    // mods.push_back(&sequencer); //Basic implementation, not very useful for now

    for (auto& module : mods) {
        module->Init();
        display.show();
    }
}

void App::Init_c1() {
    std::vector<Module*> &mods =  modules_c1;

    // parked modules
    mods.push_back(&display); //Updated implementation using DMA
    mods.push_back(&ledStrip); //Implemented correctly with PIO and DMA
    mods.push_back(&encoders); //reimplemented with polling without PIO, a bit slow but it works, includes button support

    mods.push_back(&usbAudio);
    // time critical modules on core 1
    mods.push_back(&joys); //Basic implementation using ADC polling every tick

    // Audio modules
    // mods.push_back(&synth); //Dummy module which should crash now, skipping
    // mods.push_back(&audio); //Empty module for now

    for (auto& module : mods) {
        module->Init();
    }
}

void App::Tick_c0() {
    for (auto& module : modules_c0) {
        module->Tick();
        module->Test();
    }
}

void App::Tick_c1() {
    for (auto& module : modules_c1) {
        module->Tick();
        module->Test();
    }
}