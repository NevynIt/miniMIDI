#include "App.h"
#include "pico/multicore.h"
#include "hwConfig.h"
#include "hardware/clocks.h"
#include "stdio.h"
#include "pico/sem.h"

// Define the static instance of App
App App::appInstance;
semaphore_t startup_sem;

void core1_entry()
{
    multicore_lockout_victim_init();

    sem_acquire_blocking(&startup_sem);
    mMApp.Init_c1();

    while (true)
    {
        mMApp.Tick_c1();
    }
}

int App::main()
{
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
    startup_stdio_cache->append("-----End of bootlog\n");
    mMApp.stdio.unregisterPrintCallback(caching_print_fn, mod_Stdio::debug);
    mMApp.stdio.unregisterPanicCallback(flush_cache);

    if (mMApp.sd.mounted)
        mMApp.sd.AppendFile("bootlog.txt", startup_stdio_cache->c_str(), startup_stdio_cache->length());
    else
        LOG_DEBUG(startup_stdio_cache->c_str());
    delete startup_stdio_cache;
}

void App::Init()
{
    // Initialize the modules on core 0
    Init_c0();

    //let core 1 initialise the modules and run
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
    display.Init();
    //activate display stdio output
    ledStrip.Init();
    sd.Init();
    //activate bootlog stdio output
    sequencer.Init();
    //lua.Init();
    config.load();
    encoders.Init();
    joys.Init();
    usbUart.Init();
    usbMidi.Init();

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
}

void App::Init_c1() {
    // Initialize the modules on core 1
    dsp.Init();
    //adc.Init();
    //pwm.Init();
    //i2s.Init();
    synth.Init();
    usbAudio.Init();
    usb.Init();

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