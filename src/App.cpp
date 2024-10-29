#include "App.h"

// Define the static instance of App
App App::appInstance;

App& App::GetInstance() {
    return appInstance;
}

void App::Init() {
    // Add existing module instances to the appropriate vectors
    modules_c0.push_back(&config);
    modules_c1.push_back(&uartMIDI);
    modules_c1.push_back(&display);
    modules_c1.push_back(&ledStrip);
    modules_c1.push_back(&usb);
    modules_c1.push_back(&usbUart);
    modules_c1.push_back(&usbMidi);
    modules_c1.push_back(&usbAudio);
    modules_c1.push_back(&sd);
    modules_c1.push_back(&encoders);
    modules_c1.push_back(&joys);
    modules_c1.push_back(&sequencer);
    modules_c1.push_back(&audio);
    modules_c1.push_back(&dsp);
}

void App::Tick() {
    // Empty, as Tick_c0 and Tick_c1 will handle ticking
}

void App::Init_c0() {
    for (auto& module : modules_c0) {
        module->Init();
    }
}

void App::Init_c1() {
    for (auto& module : modules_c1) {
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