#include "App.h"
#include "pico/multicore.h"

void core1_entry() {
    App& app = App::GetInstance();

    app.Init_c1();
    
    while (true) {
        app.Tick_c1();
    }
}

// extern "C"
// {
// int i2s_example_main();
// }

// int main() {

//     i2s_example_main();
//     return 0;
// }

int main() {
    App& app = App::GetInstance();

    //Initialise the modules on core 0 first
    app.Init_c0();

    multicore_launch_core1(core1_entry); //do not start the other modules for now

    while (true) {
        app.Tick_c0();

        app.display.clear();
        char buffer[100];
        sprintf(buffer, "%d, %d, %d, %d, %d\n",  app.encoders.buttons, app.encoders.count[0]/4, app.encoders.count[1]/4, app.encoders.count[2]/4, app.encoders.count[3]/4);
        app.display.draw_text(0,16,1,buffer);
        sprintf(buffer, "%d, %d, %d\n", app.joys.button, app.joys.GetX()>>3, app.joys.GetY()>>3);
        app.display.draw_text(0,24,1,buffer);

        static uint8_t mode = 0;

        uint8_t btn = app.encoders.buttons;
        
        switch (btn)
        {
        case 1:
            mode = 0;
            break;
        case 2:
            mode = 1;
            break;
        case 4:
            mode = 2;
            break;
        case 8:
            mode = 3;
            break;
        }

        uint8_t i,r,g,b,t;

        switch (mode)
        {
        case 0:
            app.ledStrip.Clear();
            i = (app.encoders.count[0]/4) % LEDS_COUNT;
            r = (app.encoders.count[1]/4);
            g = (app.encoders.count[2]/4);
            b = (app.encoders.count[3]/4);
            app.ledStrip.SetColor(i,r,g,b);
            break;
        case 1:
            app.ledStrip.Clear();
            i = (app.encoders.count[0]/4) % LEDS_COUNT;
            t = (128*8 + app.encoders.count[1]/4) % 128;
            t & 0b01111110;
            b = (app.encoders.count[2]/4) % 128;
            app.ledStrip.SetColor(i,t,b);
            break;
        case 2:
            i = rand() % LEDS_COUNT;
            t = rand() % 128;
            b = (app.encoders.count[2]/4) % 128;
            app.ledStrip.SetColor(i,t,b);
            break;
        case 3:
            for (i = 0; i < LEDS_COUNT; i++)
            {
                t = (i + app.encoders.count[0]/4) % 128;
                b = (app.encoders.count[2]/4) % 128;
                app.ledStrip.SetColor(i,t,b);
            }
            break;
        default:
            break;
        }

    }

    return 0;
}
