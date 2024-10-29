#ifndef MOD_AUDIO_H
#define MOD_AUDIO_H

#include "Module.h"
#include "i2s.h"

class mod_Audio : public Module {
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

#endif // MOD_AUDIO_H