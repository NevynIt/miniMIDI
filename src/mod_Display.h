#ifndef MOD_DISPLAY_H
#define MOD_DISPLAY_H

#include "Module.h"

#include <vector>
#include <cstdint>
#include <string>
#include "hwConfig.h"
#include "hardware/dma.h"

//based on the work of David Schramm (c) 2021
//pico-ssd1306 library

class mod_Display : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    void clear();
    void show();
    void draw_pixel(int x, int y);
    void clear_pixel(int x, int y);
    void draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void draw_square(int x, int y, int w, int h);
    void clear_square(int x, int y, int w, int h);
    void draw_text(int x, int y, uint32_t scale, const char* text);
    void scroll(int dy);
    // void draw_bitmap(int x, int y, const uint8_t* bitmap, int w, int h);
    // void set_contrast(uint8_t contrast);
    // void invert_display(bool invert);
    
    // Cursor management and printing
    void setCursor(int x, int y);
    void print(const std::string& text);
    void println(const std::string& text);

    void draw_char_with_font(int x, int y, uint32_t scale, const uint8_t *font, char c);
    void draw_string_with_font(int x, int y, uint32_t scale, const uint8_t *font, const char *s);
    void draw_char(int x, int y, uint32_t scale, char c);
    void draw_string(int x, int y, uint32_t scale, const char *s);

private:
    int dma_channel = -1;
    dma_channel_config dma_cfg;

    uint8_t frame[DISPLAY_BUFSIZE] = {0};
    uint16_t buffer[DISPLAY_BUFSIZE + 13] = {0}; //13 entries to initialise the refresh command before sending the buffer
    
    int cursor_x=0;
    int cursor_y=0;
};

#endif // MOD_DISPLAY_H