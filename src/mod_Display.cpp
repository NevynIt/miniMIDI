#include "mod_Display.h"
#include <cstring>
#include <algorithm>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

void mod_Display::Init() {
    // Initialize I2C and SSD1306 display
    i2c_init(I2C_DISPLAY, 400000);
    gpio_set_function(GPIO_Display_SDA, GPIO_FUNC_I2C);
    gpio_set_function(GPIO_Display_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(GPIO_Display_SDA);
    gpio_pull_up(GPIO_Display_SCL);

    // Initialize the display
    display.external_vcc = false;
    ssd1306_init(&display, WIDTH, HEIGHT, 0x3C, i2c1);
    clear();
    show();
}

void mod_Display::Tick() {
    // Perform any periodic display tasks if needed
}

void mod_Display::clear() {
    ssd1306_clear(&display);
}

void mod_Display::show() {
    ssd1306_show(&display);
}

void mod_Display::draw_pixel(int x, int y) {
    ssd1306_draw_pixel(&display, x, y);
}

void mod_Display::clear_pixel(int x, int y) {
    ssd1306_clear_pixel(&display, x, y);
}

void mod_Display::draw_line(int x0, int y0, int x1, int y1) {
    ssd1306_draw_line(&display, x0, y0, x1, y1);
}

void mod_Display::draw_square(int x, int y, int w, int h) {
    ssd1306_draw_square(&display, x, y, w, h);
}

void mod_Display::clear_square(int x, int y, int w, int h) {
    ssd1306_clear_square(&display, x, y, w, h);
}

void mod_Display::draw_text(int x, int y, uint32_t scale, const char* text) {
    ssd1306_draw_string(&display, x, y, scale, text);
}

// void mod_Display::draw_bitmap(int x, int y, const uint8_t* bitmap) {
//     ssd1306_bmp_show_image_with_offset(&display, x, y, bitmap);
// }

// void mod_Display::set_contrast(uint8_t contrast) {
//     ssd1306_set_contrast(&display, contrast);
// }

// void mod_Display::invert_display(bool invert) {
//     ssd1306_invert_display(&display, invert);
// }