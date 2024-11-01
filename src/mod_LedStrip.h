#ifndef MOD_LEDSTRIP_H
#define MOD_LEDSTRIP_H

#include "Module.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include <vector>
#include "hwConfig.h"

class mod_LedStrip : public Module {
public:
    void Init() override;
    void Tick() override;

    void SetColor(uint index, uint32_t color);
    void SetColor(uint index, uint8_t r, uint8_t g, uint8_t b);
    void SetColor(uint index, uint8_t tone, uint8_t brightness);
    void Clear();

    static uint32_t hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v);
    static uint32_t pixel_map(uint index);
private:
    uint32_t led_buffer[LEDS_COUNT] = {0};

    int sm;
    int dma_channel;
    uint offset;
};

#endif // MOD_LEDSTRIP_H