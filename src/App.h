#ifndef APP_H
#define APP_H

#include "Module.h"
#include "mod_UART_MIDI.h"
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
#include "mod_Audio.h"
#include "mod_DSP.h"
#include "mod_Synth.h"
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
    mod_Display display;
    mod_LedStrip ledStrip;
    mod_USB usb;
    mod_USB_UART usbUart;
    mod_USB_MIDI usbMidi;
    mod_USB_Audio usbAudio;
    mod_SD sd;
    mod_Encoders encoders;
    mod_Joys joys;
    mod_Sequencer sequencer;
    mod_Audio audio;
    mod_DSP dsp;
    mod_Synth synth;

private:
    App() = default; // Private constructor for singleton pattern
    static App appInstance;

    std::vector<Module*> modules_c0; // Modules to run on core 0
    std::vector<Module*> modules_c1; // Modules to run on core 1
};

#endif // APP_H