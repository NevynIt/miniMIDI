#ifndef MOD_LEDSTRIP_H
#define MOD_LEDSTRIP_H

#include "Module.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include <vector>

class mod_LedStrip : public Module {
public:
    void Init() override;
    void Tick() override;

    void SetColor(uint index, uint8_t h, uint8_t s, uint8_t v);
    void Update();

private:
    static const int NUM_PIXELS = 64;
    static const int WS2812_PIN = 2;
    std::vector<uint32_t> led_buffer;
    uint32_t hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v);
    uint32_t pixel_map(uint index);

    PIO pio;
    int sm;
    int dma_channel;
    uint offset;
};

#endif // MOD_LEDSTRIP_H