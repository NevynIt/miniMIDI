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

class startup_target : public buffering_target
{
public:
    startup_target() : buffering_target(4096) { }

    void panic_fn() override
    {
        flush_cache();
    }

    void flush_cache()
    {
        if (buffer.length() > 0)
        {
            buffer.clear();
        }
    }
} *startup_stdio_cache;

void App::Init()
{
    lua.PreInit(); //every other module will add to the Lua mM table
    stdio.Init(); //Init these first of all

    startup_stdio_cache = new startup_target();
    startup_stdio_cache->buffer.append("Booting...\n");
    stdio.registerTarget(startup_stdio_cache);

    stdio.setLogChannel(LogChannel::lc_boot);

    // Initialize the modules on core 0
    Init_c0();

    //let core 1 initialise the modules
    sem_release(&startup_sem);

    sleep_ms(100);

    //wait for core 1 to finish initialisation
    sem_acquire_blocking(&startup_sem);

    //deactivate bootlog stdio output
    stdio.unregisterTarget(startup_stdio_cache);
    startup_stdio_cache->flush_cache();
    delete startup_stdio_cache;

    stdio.setLogChannel(LogChannel::lc_none);

    //let core 1 go free
    sem_release(&startup_sem);
}

void App::Init_c0()
{
    config.Init();
    hwConfig = config.GetConfig<hw_cfg>("hw_config");
    if (hwConfig == nullptr || hwConfig->version != compiled_hw_cfg_version)
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
    sd.Init();

    lua.Init();

    //lower priority
    encoders.Init(); //todo: reading configuration information from flash
    joys.Init(); //todo: reading configuration information from flash, and merge with the encoders in a single module
    usbCdc.Init();
    usbMidi.Init(); //todo: everything

    //next priority
    usbMsc.Init();      //todo:
                        //present one read/write drive with the sd card -- DONE
                        //present one readonly drive with an image of the flash memory, separated between the program and the configuration
    usbAudio.Init();    //todo: testing the input interface and implementing the volume control to the onboard speaker?
    usb.Init();         //add support for the usb drive interface usb configuration

    // non time critical modules run on core 0
    std::vector<Module*> &mods =  modules_c0;

    mods.push_back(&stdio);
    mods.push_back(&blink);
    mods.push_back(&config);
    mods.push_back(&sd);
    mods.push_back(&uart);
    mods.push_back(&display);
    mods.push_back(&ledStrip);
    // mods.push_back(&lua);
    mods.push_back(&encoders);
    mods.push_back(&joys);
    mods.push_back(&usbCdc);
    mods.push_back(&usbMidi);
    mods.push_back(&usbMsc);
    mods.push_back(&usb);
    mods.push_back(&usbAudio);
}

void App::Init_c1() {
    //wait for core 0 to finish initialisation
    sem_acquire_blocking(&startup_sem);

    // Initialize the modules on core 1
    //adc.Init(); //todo: everything from scratch
    //i2s.Init(); //todo: everything from scratch
    //pwm.Init(); //todo: everything from scratch
    sequencer.Init(); //todo: everything, the module is empty at the moment
    synth.Init(); //todo: moving from wave generators to instruments and linking to midi
    dsp.Init(); //todo: a lot, including testing and finalising the use of the new asm and processing buffers with it

    // time critical modules run on core 1
    std::vector<Module*> &mods =  modules_c1;

    // modules_c1.push_back(&adc);
    // modules_c1.push_back(&i2s);
    // modules_c1.push_back(&pwm);
    mods.push_back(&sequencer);
    mods.push_back(&synth);
    mods.push_back(&dsp);

    // let core 0 run
    sem_release(&startup_sem);

    sleep_ms(100);

    //wait for core 1 to remove the boot log target
    sem_acquire_blocking(&startup_sem);
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