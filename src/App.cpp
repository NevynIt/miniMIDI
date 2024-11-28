#include "App.h"
#include "pico/multicore.h"
#include "hwConfig.h"
#include "hardware/clocks.h"
#include "stdio.h"
#include "pico/sem.h"

App App::appInstance;
semaphore_t startup_sem;

void core1_entry()
{
    multicore_lockout_victim_init();

    mMApp.Init_c1();

    while (true)
    {
        mMApp.Tick_c1();
    }
}

int App::main()
{
    // prevent core 1 from running until core 0 is ready
    sem_init(&startup_sem, 0, 1);

    multicore_launch_core1(core1_entry);

    Init();

    while (true)
    {
        Tick_c0();
    }
}

std::string *startup_stdio_cache;

void caching_print_fn(const char *buf, int len)
{
    startup_stdio_cache->append(buf, len);
}

void flush_cache()
{
    mMApp.stdio.unregisterPrintCallback(caching_print_fn, mod_Stdio::debug);
    mMApp.stdio.unregisterPanicCallback(flush_cache);
    startup_stdio_cache->append("-----End of bootlog\n");

    if (mMApp.sd.mounted)
        mMApp.sd.WriteFile("bootlog.txt", startup_stdio_cache->c_str(), startup_stdio_cache->length());
    else
        LOG_DEBUG(startup_stdio_cache->c_str());
    delete startup_stdio_cache;
}

void App::Init()
{
    // Initialize the modules on core 0
    Init_c0();

    //let core 1 initialise the modules
    sem_release(&startup_sem);

    //wait for core 1 to finish initialisation
    sem_acquire_blocking(&startup_sem);

    //deactivate bootlog stdio output
    flush_cache();
}

void App::Init_c0()
{
    stdio.Init();
    
    //cache the bootlog
    startup_stdio_cache = new std::string();
    startup_stdio_cache->append("-----Bootlog:\n");

    stdio.registerPrintCallback(caching_print_fn, mod_Stdio::debug);
    stdio.registerPanicCallback(flush_cache);
    
    config.Init();
    hwConfig = config.GetConfig<hw_cfg>("hw_config");
    if (hwConfig == nullptr)
    {
        LOG_WARNING("hw_config not found\n");
        hwConfig = new hw_cfg();
        config.SetConfig("hw_config", *hwConfig);
    }

    set_sys_clock_khz(hwConfig->cpu_khz, true);

    blink.Init();
    uart.Init();
    //done up to here

    //lower priority
    display.Init(); //todo: reading configuration information from flash and doing a splash screen
    //activate display stdio output //todo
    ledStrip.Init(); //todo: reading configuration information from flash and doing a visual effect on the leds
    sd.Init(); //todo: reading configuration information from flash
    sequencer.Init(); //todo: everything, the module is empty at the moment

    //medium priority
    //lua.Init(); //todo: separate lua from uart, and make it a module
    //lua.load_config(); //todo: run the configuration scripts from the sd, and update the configuration as needed

    //lower priority
    encoders.Init(); //todo: reading configuration information from flash
    joys.Init(); //todo: reading configuration information from flash, and merge with the encoders in a single module
    usbUart.Init(); //todo: everything
    usbMidi.Init(); //todo: everything

    //next priority
    //usbMSC.Init(); //todo: everything from scratch, must update the sd module first to access r/w the blocks one by one
                     //present one readonly drive with an image of the flash memory, separated between the program and the configuration
                     //present one read/write drive with the sd card
                     //it should be pretty easy actually

    // non time critical modules run on core 0
    std::vector<Module*> &mods =  modules_c0;

    mods.push_back(&blink);
    // modules_c0.push_back(&stdio);
    mods.push_back(&uart);
    mods.push_back(&display);
    mods.push_back(&ledStrip);
    mods.push_back(&sequencer);
    // modules_c0.push_back(&lua);
    mods.push_back(&encoders);
    mods.push_back(&joys);
    mods.push_back(&usbUart);
    mods.push_back(&usbMidi);
    // modules_c0.push_back(&usbMSC);
}

void App::Init_c1() {
    //wait for core 0 to finish initialisation
    sem_acquire_blocking(&startup_sem);

    // Initialize the modules on core 1
    dsp.Init(); //todo: a lot, including testing and finalising the use of the new asm and processing buffers with it
    //adc.Init(); //todo: everything from scratch
    //pwm.Init(); //todo: everything from scratch
    //i2s.Init(); //todo: everything from scratch
    synth.Init(); //todo: moving from wave generators to instruments and linking to midi
    usbAudio.Init(); //todo: testing the input interface and implementing the volume control to the onboard speaker?
    usb.Init(); //add support for the usb drive interface usb configuration

    // let core 0 run
    sem_release(&startup_sem);

    // time critical modules run on core 1
    std::vector<Module*> &mods =  modules_c1;

    mods.push_back(&dsp);
    // modules_c1.push_back(&adc);
    // modules_c1.push_back(&pwm);
    // modules_c1.push_back(&i2s);
    mods.push_back(&synth);
    mods.push_back(&usb);
    mods.push_back(&usbAudio);
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