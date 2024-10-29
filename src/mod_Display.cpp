#include "mod_Display.h"
#include <cstring>
#include <algorithm>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

void mod_Display::Init() {
    // Initialize I2C and SSD1306 display
    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    // Initialize the framebuffer
    framebuffer.resize(WIDTH * HEIGHT / 8, 0);

    // Initialize the display
    SendCommand(0xAE); // Display off
    SendCommand(0xD5); // Set display clock divide ratio/oscillator frequency
    SendCommand(0x80); // Set divide ratio
    SendCommand(0xA8); // Set multiplex ratio(1 to 64)
    SendCommand(0x3F); // 1/64 duty
    SendCommand(0xD3); // Set display offset
    SendCommand(0x00); // No offset
    SendCommand(0x40 | 0x0); // Set start line address
    SendCommand(0x8D); // Enable charge pump regulator
    SendCommand(0x14);
    SendCommand(0x20); // Set Memory Addressing Mode
    SendCommand(0x00); // Horizontal addressing mode
    SendCommand(0xA1); // Set segment re-map 0 to 127
    SendCommand(0xC8); // Set COM output scan direction
    SendCommand(0xDA); // Set COM pins hardware configuration
    SendCommand(0x12);
    SendCommand(0x81); // Set contrast control register
    SendCommand(0xCF);
    SendCommand(0xD9); // Set pre-charge period
    SendCommand(0xF1);
    SendCommand(0xDB); // Set VCOMH deselect level
    SendCommand(0x40);
    SendCommand(0xA4); // Entire display ON
    SendCommand(0xA6); // Set normal display
    SendCommand(0xAF); // Display ON
}

void mod_Display::Tick() {
    // Perform any periodic display tasks if needed
}

void mod_Display::Clear() {
    std::fill(framebuffer.begin(), framebuffer.end(), 0);
}

void mod_Display::DrawPixel(int x, int y, bool color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    if (color) {
        framebuffer[x + (y / 8) * WIDTH] |= (1 << (y % 8));
    } else {
        framebuffer[x + (y / 8) * WIDTH] &= ~(1 << (y % 8));
    }
}

void mod_Display::DrawLine(int x0, int y0, int x1, int y1, bool color) {
    // Bresenham's line algorithm
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (true) {
        DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void mod_Display::DrawRect(int x, int y, int w, int h, bool color) {
    DrawLine(x, y, x + w - 1, y, color);
    DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
    DrawLine(x + w - 1, y + h - 1, x, y + h - 1, color);
    DrawLine(x, y + h - 1, x, y, color);
}

void mod_Display::FillRect(int x, int y, int w, int h, bool color) {
    for (int i = x; i < x + w; ++i) {
        for (int j = y; j < y + h; ++j) {
            DrawPixel(i, j, color);
        }
    }
}

void mod_Display::DrawText(int x, int y, const std::string& text, bool color) {
    // Implement text drawing using a simple font
    // This is a placeholder implementation
    for (char c : text) {
        // Draw each character (assuming 8x8 font)
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                DrawPixel(x + i, y + j, color);
            }
        }
        x += 8; // Move to the next character position
    }
}

void mod_Display::Update() {
    // Send the framebuffer to the display
    SendCommand(0x21); // Set column address
    SendCommand(0);    // Column start address
    SendCommand(WIDTH - 1); // Column end address
    SendCommand(0x22); // Set page address
    SendCommand(0);    // Page start address
    SendCommand((HEIGHT / 8) - 1); // Page end address

    SendData(framebuffer);
}

void mod_Display::SendCommand(uint8_t command) {
    uint8_t buf[2] = {0x00, command};
    i2c_write_blocking(i2c0, 0x3C, buf, 2, false);
}

void mod_Display::SendData(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> buf(data.size() + 1);
    buf[0] = 0x40;
    std::copy(data.begin(), data.end(), buf.begin() + 1);
    i2c_write_blocking(i2c0, 0x3C, buf.data(), buf.size(), false);
}