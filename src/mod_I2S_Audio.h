#ifndef mod_I2S_Audio_H
#define mod_I2S_Audio_H

#include "Module.h"
#include "i2s.h"

class mod_I2S_Audio : public Module {
public:
    void Init() override;
    void Tick() override;

    void Start();
    void Stop();
    void SetDMAHandler(void (*dma_handler)(void));

private:
    pio_i2s i2s_instance;
    static void DefaultDMAHandler();
};

#endif // mod_I2S_Audio_H