#ifndef APP_H
#define APP_H

#include "Module.h"
#include "mod_UART.h"
#include "mod_config.h"
#include "mod_Display.h"
#include "mod_LedStrip.h"
#include "mod_USB.h"
#include "mod_USB_UART.h"
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
#include <vector>
#include <memory>
#include "hardware/timer.h"

        // INTERVALCHECK_stored_time += (interval); \
        //

#define mMApp (App::GetInstance())

inline bool INTERVALCHECK_IMP(uint32_t interval, uint32_t& INTERVALCHECK_stored_time) {
    uint32_t now = time_us_32();
    if (now-INTERVALCHECK_stored_time > (interval) * 1000) {
        while (now-INTERVALCHECK_stored_time > (interval) * 1000)
            INTERVALCHECK_stored_time += (interval) * 1000;
        return false;
    }
    return true;
}

#define INTERVALCHECK(interval) \
{\
    static uint32_t INTERVALCHECK_stored_time = 0; \
    if (INTERVALCHECK_IMP(interval, INTERVALCHECK_stored_time)) \
        return;\
}

class App : public Module {
public:
    static inline App& GetInstance() { return appInstance; }

    void Init() override;
    void Tick() override;

    void Init_c0();
    void Init_c1();
    void Tick_c0();
    void Tick_c1();

    mod_I2S_Audio audio;
    mod_Blink blink;
    mod_config config;
    mod_Display display;
    mod_DSP dsp;
    mod_Encoders encoders;
    mod_Joys joys;
    mod_LedStrip ledStrip;
    mod_SD sd;
    mod_Sequencer sequencer;
    mod_Synth synth;
    mod_UART uart;
    mod_USB usb;
    mod_USB_Audio usbAudio;
    mod_USB_MIDI usbMidi;
    mod_USB_UART usbUart;

private:
    App() = default; // Private constructor for singleton pattern
    static App appInstance;

    std::vector<Module*> modules_c0; // Modules to run on core 0
    std::vector<Module*> modules_c1; // Modules to run on core 1
};

#endif // APP_H