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
    
    cursor_x = 0;
    cursor_y = 0;

    // Initialize the display
    display.external_vcc = false;
    ssd1306_init(&display, WIDTH, HEIGHT, 0x3C, I2C_DISPLAY);
    clear();
    println("Boot sequence started");
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

void mod_Display::scroll(int dy) {
    if (dy == 0) return; // No scrolling needed

    if (abs(dy) >= HEIGHT) {
        // Scrolling by more than the display height, clear display
        clear();
        return;
    }

    int total_rows = HEIGHT;
    int page_height = 8;
    int pages = display.pages; // Number of pages (HEIGHT / 8)
    int page_offset = dy / page_height;
    int bit_offset = dy % page_height;

    if (bit_offset < 0) {
        bit_offset += 8;
        page_offset -= 1;
    }

    if (dy > 0) {
        // Scroll down
        int byte_offset = page_offset * WIDTH;
        if (byte_offset > 0) {
            // Shift pages
            memmove(display.buffer + byte_offset,
                    display.buffer,
                    display.bufsize - byte_offset);
            memset(display.buffer, 0, byte_offset);
        }

        if (bit_offset > 0) {
            // Shift bits within pages
            for (int x = 0; x < WIDTH; x++) {
                uint8_t carry = 0;
                for (int page = pages - 1; page >= 0; page--) {
                    int index = page * WIDTH + x;
                    uint8_t current_byte = display.buffer[index];
                    display.buffer[index] = (current_byte << bit_offset) | carry;
                    carry = current_byte >> (8 - bit_offset);
                }
            }
        }

        // Clear the new area at the top
        int rows_to_clear = dy;
        clear_square(0, 0, WIDTH, rows_to_clear);

    } else {
        // Scroll up
        dy = -dy;
        page_offset = dy / page_height;
        bit_offset = dy % page_height;

        if (bit_offset < 0) {
            bit_offset += 8;
            page_offset -= 1;
        }

        int byte_offset = page_offset * WIDTH;
        if (byte_offset > 0) {
            // Shift pages
            memmove(display.buffer,
                    display.buffer + byte_offset,
                    display.bufsize - byte_offset);
            //memset(display.buffer + display.bufsize - byte_offset, 0, byte_offset);
        }

        if (bit_offset > 0) {
            // Shift bits within pages
            for (int x = 0; x < WIDTH; x++) {
                uint8_t carry = 0;
                for (int page = 0; page < pages; page++) {
                    int index = page * WIDTH + x;
                    uint8_t current_byte = display.buffer[index];
                    display.buffer[index] = (current_byte >> bit_offset) | carry;
                    carry = current_byte << (8 - bit_offset);
                }
            }
        }

        // Clear the new area at the bottom
        int rows_to_clear = dy;
        //clear_square(0, HEIGHT - rows_to_clear, WIDTH, rows_to_clear);
    }
}

void mod_Display::setCursor(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

void mod_Display::print(const std::string& text) {
    draw_text(cursor_x, cursor_y, 1, text.c_str());
}

void mod_Display::println(const std::string& text) {
    if (cursor_y+8 >= HEIGHT) {
        scroll(-8);
        cursor_y -= 8;
    }
    print(text);
    cursor_x = 0;
    cursor_y += 8;
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