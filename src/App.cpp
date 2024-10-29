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