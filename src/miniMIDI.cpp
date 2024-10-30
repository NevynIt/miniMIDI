#include "App.h"
#include "pico/multicore.h"

void core1_entry() {
    App& app = App::GetInstance();

    app.Init_c1();
    
    while (true) {
        app.Tick_c1();
    }
}

int main() {
    App& app = App::GetInstance();

    //Initialise the modules on core 0 first
    app.Init_c0();

    //multicore_launch_core1(core1_entry); //do not start the other modules for now

    while (true) {
        app.Tick_c0();
    }

    return 0;
}
