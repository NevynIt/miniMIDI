#include "mod_LedStrip.h"
#include "ws2812.pio.h"
#include <cmath>

void mod_LedStrip::Init() {
    // Initialize PIO and DMA
    pio = pio0;
    sm = 0;
    offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false); // Added missing argument

    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, true));
    dma_channel_configure(dma_channel, &c, &pio->txf[sm], led_buffer.data(), led_buffer.size(), false);

    // Initialize LED buffer
    led_buffer.resize(NUM_PIXELS, 0);
}

void mod_LedStrip::Tick() {
    // Perform any periodic LED tasks if needed
}

void mod_LedStrip::SetColor(uint index, uint8_t h, uint8_t s, uint8_t v) {
    if (index >= NUM_PIXELS) return;
    uint32_t color = hsv_to_rgb(h, s, v);
    uint32_t mapped_index = pixel_map(index);
    led_buffer[mapped_index] = color;
}

void mod_LedStrip::Update() {
    dma_channel_set_read_addr(dma_channel, led_buffer.data(), true);
}

uint32_t mod_LedStrip::hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v) {
    float r, g, b;
    float hf = h / 255.0f * 360.0f;
    float sf = s / 255.0f;
    float vf = v / 255.0f;
    int i = static_cast<int>(hf / 60.0f) % 6;
    float f = hf / 60.0f - i;
    float p = vf * (1.0f - sf);
    float q = vf * (1.0f - f * sf);
    float t = vf * (1.0f - (1.0f - f) * sf);

    switch (i) {
        case 0: r = vf; g = t; b = p; break;
        case 1: r = q; g = vf; b = p; break;
        case 2: r = p; g = vf; b = t; break;
        case 3: r = p; g = q; b = vf; break;
        case 4: r = t; g = p; b = vf; break;
        case 5: r = vf; g = p; b = q; break;
    }

    return (static_cast<uint32_t>(r * 255) << 8) |
           (static_cast<uint32_t>(g * 255) << 16) |
           static_cast<uint32_t>(b * 255);
}

uint32_t mod_LedStrip::pixel_map(uint index) {
    // Example mapping function, you can customize this
    static const uint32_t pixel_map_table[NUM_PIXELS] = {
        // Fill this table with your specific pixel mapping
    };
    return pixel_map_table[index];
}