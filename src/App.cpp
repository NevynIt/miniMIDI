#include "App.h"

// Define the static instance of App
App App::appInstance;

App& App::GetInstance() {
    return appInstance;
}

void App::Init() {
    // Empty, as Init_c0 and Init_c1 will handle initialization
}

void App::Tick() {
    // Empty, as Tick_c0 and Tick_c1 will handle ticking
}

void App::Init_c0() {
    std::vector<Module*> &mods =  modules_c0;

    // User interface modules
    mods.push_back(&blink); // basic visual feedback of operation
    mods.push_back(&uart); //Supports STDIO for debugging
    mods.push_back(&display); //Basic implementation using a library -- very slow refresh, and keeps the CPU busy, should be updated with DMA - Scroll is buggy
    
    mods.push_back(&sd); //SD comms broken atm for some reason
    // mods.push_back(&config); //Dummy module for now

    // USB modules
    mods.push_back(&usb);
    mods.push_back(&usbUart); //Dummy module for now
    mods.push_back(&usbMidi); //Empty module for now
    mods.push_back(&usbAudio); //Empty module for now
    

    for (auto& module : mods) {
        module->Init();
    }
}

void App::Init_c1() {
    std::vector<Module*> &mods =  modules_c1;

    mods.push_back(&encoders); //reimplemented with polling without PIO, a bit slow but it works, includes button support
    mods.push_back(&joys); //Basic implementation using ADC polling every tick
    mods.push_back(&ledStrip); //Implemented correctly with PIO and DMA

    // MIDI modules
    // mods.push_back(&sequencer); //Basic implementation, not very useful for now

    // Audio modules
    // mods.push_back(&audio); //Empty module for now
    // mods.push_back(&dsp);  //Dummy module for now
    //mods.push_back(&synth); //Dummy module which should crash now, skipping

    for (auto& module : mods) {
        module->Init();
    }
}

void App::Tick_c0() {
    for (auto& module : modules_c0) {
        module->Tick();
    }
}

void App::Tick_c1() {
    for (auto& module : modules_c1) {
        module->Tick();
    }
}