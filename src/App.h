#ifndef APP_H
#define APP_H

#include "Module.h"
#include "mod_UART.h"
#include "mod_config.h"
#include "mod_Display.h"
#include "mod_LedStrip.h"
#include "mod_USB.h"
#include "mod_USB_CDC.h"
#include "mod_USB_MIDI.h"
#include "mod_USB_Audio.h"
#include "mod_SD.h"
#include "mod_Encoders.h"
#include "mod_Joys.h"
#include "mod_Sequencer.h"
#include "mod_I2S_Audio.h"
#include "mod_DSP.h"
#include "mod_Synth.h"
#include "mod_Blink.h"
#include "mod_Stdio.h"
#include "mod_USB_MSC.h"
#include "mod_Lua.h"
#include <vector>
#include <memory>
#include "hardware/timer.h"
#include "hwconfig.h"

#define mMApp (App::GetInstance())

class IntervalCheck {
    public:
        IntervalCheck(uint32_t interval) : interval(interval) {}
        bool Check() {
            uint32_t now = time_us_32();
            if (now-stored_time > (interval) * 1000) {
                while (now-stored_time > (interval) * 1000)
                    stored_time += (interval) * 1000;
                return true;
            }
            return false;
        }

    public:
        uint32_t interval;
        uint32_t stored_time = 0;
};

#define INTERVALCHECK(interval) \
{\
    static IntervalCheck INTERVALCHECK_instance(interval);\
    if (!INTERVALCHECK_instance.Check()) \
        return;\
}

class App : public Module {
public:
    static inline App& GetInstance() { return appInstance; }

    void Init() override;

    int main();

    void Init_c0();
    void Init_c1();
    void Tick_c0();
    void Tick_c1();

    mod_Lua lua;
    mod_Stdio stdio;
    mod_config config;
    mod_Blink blink;
    mod_UART uart;
    mod_Display display;
    mod_LedStrip ledStrip;
    mod_SD sd;
    mod_Encoders encoders;
    mod_Joys joys;
    mod_USB_CDC usbCdc;
    mod_USB usb;
    mod_USB_Audio usbAudio;
    mod_USB_MIDI usbMidi;
    mod_USB_MSC usbMsc;

    mod_I2S_Audio i2s;
    mod_Sequencer sequencer;
    mod_Synth synth;
    mod_DSP dsp;

    hw_cfg *hwConfig = nullptr;
private:
    App() = default; // Private constructor for singleton pattern
    static App appInstance;

    std::vector<Module*> modules_c0; // Modules to run on core 0
    std::vector<Module*> modules_c1; // Modules to run on core 1
};

#endif // APP_H