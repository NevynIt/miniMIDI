#include "mod_LedStrip.h"
#include "ws2812.pio.h"
#include <cmath>
#include "led_colors.h"
#include "time.h"
#include <string.h>
#include "stdio.h"
#include "App.h"

void mod_LedStrip::Init() {
    // Initialize PIO and DMA
    sm = 0;
    offset = pio_add_program(PIO_WS2812B, &ws2812_program);
    ws2812_program_init(PIO_WS2812B, sm, offset, GPIO_LedStrip, 800000, false);

    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(PIO_WS2812B, sm, true));
    dma_channel_configure(dma_channel, &c, &PIO_WS2812B->txf[sm], led_buffer, LEDS_COUNT, false);

    App::GetInstance().display.println("LedStrip initialized");
    printf("LedStrip initialized\n");
}

void mod_LedStrip::Tick() {
    static uint32_t stored_time = 0;
    uint32_t current_time;
    current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - stored_time > 100) {
        //update display
        dma_channel_set_read_addr(dma_channel, led_buffer, true);
        stored_time = current_time;
    }
}

void mod_LedStrip::Test() {
    App &app = App::GetInstance();

    static uint8_t mode = 0;

    uint8_t btn = app.encoders.buttons;

    switch (btn)
    {
    case 1:
        mode = 0;
        break;
    case 2:
        mode = 1;
        break;
    case 4:
        mode = 2;
        break;
    case 8:
        mode = 3;
        break;
    }

    uint8_t i, r, g, b, t;

    switch (mode)
    {
    case 0:
        app.ledStrip.Clear();
        i = (app.encoders.count[0] / 4) % LEDS_COUNT;
        r = (app.encoders.count[1] / 4);
        g = (app.encoders.count[2] / 4);
        b = (app.encoders.count[3] / 4);
        app.ledStrip.SetColor(i, r, g, b);
        break;
    case 1:
        app.ledStrip.Clear();
        i = (app.encoders.count[0] / 4) % LEDS_COUNT;
        t = (128 * 8 + app.encoders.count[1] / 4) % 128;
        t & 0b01111110;
        b = (app.encoders.count[2] / 4) % 128;
        app.ledStrip.SetColor(i, t, b);
        break;
    case 2:
        i = rand() % LEDS_COUNT;
        t = rand() % 128;
        b = (app.encoders.count[2] / 4) % 128;
        app.ledStrip.SetColor(i, t, b);
        break;
    case 3:
        for (i = 0; i < LEDS_COUNT; i++)
        {
            t = (i + app.encoders.count[0] / 4) % 128;
            b = (app.encoders.count[2] / 4) % 128;
            app.ledStrip.SetColor(i, t, b);
        }
        break;
    default:
        break;
    }
}

void mod_LedStrip::SetColor(uint index, uint8_t r, uint8_t g, uint8_t b) {
    SetColor(index, (static_cast<uint32_t>(r) << 8) | (static_cast<uint32_t>(g) << 16) |static_cast<uint32_t>(b));
}

void mod_LedStrip::SetColor(uint index, uint32_t color)
{
    if (index >= LEDS_COUNT) return;
    uint32_t mapped_index = pixel_map(index);
    led_buffer[mapped_index] = color << 8;
}

void mod_LedStrip::SetColor(uint index, uint8_t tone, uint8_t brightness)
{
    if (tone >= 128 || brightness >= 128) return;
    SetColor(index, LedColor[tone][brightness]);
}

void mod_LedStrip::Clear()
{
    memset(led_buffer, 0, sizeof(led_buffer));
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

uint32_t inline mod_LedStrip::pixel_map(uint index) {
    // Example mapping function, you can customize this
    static const uint32_t pixel_map_table[LEDS_COUNT] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22
    };
    return pixel_map_table[index];
}