#include "mod_Display.h"
#include <cstring>
#include <algorithm>
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "time.h"
#include "App.h"

typedef enum {
    SET_CONTRAST = 0x81,
    SET_ENTIRE_ON = 0xA4,
    SET_NORM_INV = 0xA6,
    SET_DISP = 0xAE,
    SET_MEM_ADDR = 0x20,
    SET_COL_ADDR = 0x21,
    SET_PAGE_ADDR = 0x22,
    SET_DISP_START_LINE = 0x40,
    SET_SEG_REMAP = 0xA0,
    SET_MUX_RATIO = 0xA8,
    SET_COM_OUT_DIR = 0xC0,
    SET_DISP_OFFSET = 0xD3,
    SET_COM_PIN_CFG = 0xDA,
    SET_DISP_CLK_DIV = 0xD5,
    SET_PRECHARGE = 0xD9,
    SET_VCOM_DESEL = 0xDB,
    SET_CHARGE_PUMP = 0x8D
} ssd1306_command_t;

const uint8_t InitCMDS[] = {
        SET_DISP,
        // timing and driving scheme
        SET_DISP_CLK_DIV,
        0x80,
        SET_MUX_RATIO,
        DISPLAY_HEIGHT - 1,
        SET_DISP_OFFSET,
        0x00,
        // resolution and layout
        SET_DISP_START_LINE,
        // charge pump
        SET_CHARGE_PUMP,
        0?0x10:0x14,
        SET_SEG_REMAP | 0x01,           // column addr 127 mapped to SEG0
        SET_COM_OUT_DIR | 0x08,         // scan from COM[N] to COM0
        SET_COM_PIN_CFG,
        DISPLAY_WIDTH>2*DISPLAY_HEIGHT?0x02:0x12,
        // display
        SET_CONTRAST,
        0xff,
        SET_PRECHARGE,
        0?0x22:0xF1,
        SET_VCOM_DESEL,
        0x30,                           // or 0x40?
        SET_ENTIRE_ON,                  // output follows RAM contents
        SET_NORM_INV,                   // not inverted
        SET_DISP | 0x01,
        // address setting
        SET_MEM_ADDR,
        0x00,  // horizontal
};

inline static void ssd1306_write(uint8_t val) {
    uint8_t d[2]= {0x00, val};
    i2c_write_blocking(I2C_DISPLAY, DISPLAY_ADDR, d, 2, false);
}

const uint8_t refresh_Cmds[] = {SET_COL_ADDR, 0, DISPLAY_WIDTH-1, SET_PAGE_ADDR, 0, (DISPLAY_HEIGHT/8)-1};

void mod_Display::Init() {
    // Initialize I2C and SSD1306 display
    i2c_init(I2C_DISPLAY, 400000);
    gpio_set_function(GPIO_Display_SDA, GPIO_FUNC_I2C);
    gpio_set_function(GPIO_Display_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(GPIO_Display_SDA);
    gpio_pull_up(GPIO_Display_SCL);
    
    // Initialize the display update commands
    frame = buffer + 1;
    buffer[0] = 0x40;

    // Initialize the display
    for (size_t i = 0; i < sizeof(InitCMDS); i++) {
        ssd1306_write(InitCMDS[i]);
    }

    //Prepare for refresh
    for (size_t i = 0; i < sizeof(refresh_Cmds); i++) {
        ssd1306_write(refresh_Cmds[i]);
    }

    // Configure I2C for DMA
    i2c_get_hw(I2C_DISPLAY)->dma_cr = I2C_IC_DMA_CR_TDMAE_BITS; // Enable I2C TX DMA

    // Claim a free DMA channel
    dma_channel = dma_claim_unused_channel(true);

    // Get default DMA channel configuration
    dma_cfg = dma_channel_get_default_config(dma_channel);

    // Configure DMA channel
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_8); // 8-bit transfers
    channel_config_set_read_increment(&dma_cfg, true);           // Increment read address
    channel_config_set_write_increment(&dma_cfg, false);         // Don't increment write address
    channel_config_set_dreq(&dma_cfg, i2c_get_dreq(I2C_DISPLAY, true)); // Set DREQ for I2C TX

    // Configure the DMA channel (don't start yet)
    dma_channel_configure(
        dma_channel,
        &dma_cfg,
        &i2c_get_hw(I2C_DISPLAY)->data_cmd, // Write to I2C data command register
        buffer,                             // Read from buffer
        sizeof(buffer),                     
        false                               // Don't start yet
    );

    clear();
    println("Boot sequence started");
    show();
}

void mod_Display::show() {
    // i2c_write_blocking(I2C_DISPLAY, DISPLAY_ADDR, buffer, sizeof(buffer), false);
    dma_channel_start(dma_channel);
}

void mod_Display::Tick() {
    static uint32_t stored_time = 0;
    uint32_t current_time;
    current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - stored_time > DISPLAY_REFRESH_MS) {
        //update display
        show();
        stored_time = current_time;
    }
}

void mod_Display::clear() {
    memset(frame, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8);
    cursor_x = 0;
    cursor_y = 0;
}

void mod_Display::draw_pixel(int x, int y) {
    if(x>=DISPLAY_WIDTH || y>=DISPLAY_HEIGHT) return;

    frame[x+DISPLAY_WIDTH*(y>>3)]|=0x1<<(y&0x07); // y>>3==y/8 && y&0x7==y%8
}

void mod_Display::clear_pixel(int x, int y) {
    if(x>=DISPLAY_WIDTH || y>=DISPLAY_HEIGHT) return;

    frame[x+DISPLAY_WIDTH*(y>>3)] &= ~(0x1<<(y&0x07));
}

inline static void swap(int32_t *a, int32_t *b) {
    int32_t *t=a;
    *a=*b;
    *b=*t;
}

void mod_Display::draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    if(x1>x2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }

    if(x1==x2) {
        if(y1>y2)
            swap(&y1, &y2);
        for(int32_t i=y1; i<=y2; ++i)
            draw_pixel(x1, i);
        return;
    }

    float m=(float) (y2-y1) / (float) (x2-x1);

    for(int32_t i=x1; i<=x2; ++i) {
        float y=m*(float) (i-x1)+(float) y1;
        draw_pixel(i, (uint32_t) y);
    }
}

void mod_Display::draw_square(int x, int y, int w, int h) {
    for(uint32_t i=0; i<w; ++i)
        for(uint32_t j=0; j<h; ++j)
            draw_pixel(x+i, y+j);
}

void mod_Display::clear_square(int x, int y, int w, int h) {
    for(uint32_t i=0; i<w; ++i)
        for(uint32_t j=0; j<h; ++j)
            clear_pixel(x+i, y+j);
}

void mod_Display::scroll(int dy) {
    if (dy == 0) return; // No scrolling needed

    if (abs(dy) >= DISPLAY_HEIGHT) {
        // Scrolling by more than the display height, clear display
        clear();
        return;
    }

    int total_rows = DISPLAY_HEIGHT;
    int page_height = 8;
    int pages = DISPLAY_HEIGHT/8; // Number of pages (HEIGHT / 8)
    int page_offset = dy / page_height;
    int bit_offset = dy % page_height;

    if (bit_offset < 0) {
        bit_offset += 8;
        page_offset -= 1;
    }

    if (dy > 0) {
        // Scroll down
        int byte_offset = page_offset * DISPLAY_WIDTH;
        if (byte_offset > 0) {
            // Shift pages
            memmove(frame + byte_offset,
                    frame,
                    (DISPLAY_WIDTH*DISPLAY_HEIGHT/8) - byte_offset);
            memset(frame, 0, byte_offset);
        }

        if (bit_offset > 0) {
            // Shift bits within pages
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                uint8_t carry = 0;
                for (int page = pages - 1; page >= 0; page--) {
                    int index = page * DISPLAY_WIDTH + x;
                    uint8_t current_byte = frame[index];
                    frame[index] = (current_byte << bit_offset) | carry;
                    carry = current_byte >> (8 - bit_offset);
                }
            }
        }

        // Clear the new area at the top
        int rows_to_clear = dy;
        clear_square(0, 0, DISPLAY_WIDTH, rows_to_clear);

    } else {
        // Scroll up
        dy = -dy;
        page_offset = dy / page_height;
        bit_offset = dy % page_height;

        if (bit_offset < 0) {
            bit_offset += 8;
            page_offset -= 1;
        }

        int byte_offset = page_offset * DISPLAY_WIDTH;
        if (byte_offset > 0) {
            // Shift pages
            memmove(frame,
                    frame + byte_offset,
                    (DISPLAY_WIDTH*DISPLAY_HEIGHT/8) - byte_offset);
            //memset(display.buffer + display.bufsize - byte_offset, 0, byte_offset);
        }

        if (bit_offset > 0) {
            // Shift bits within pages
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                uint8_t carry = 0;
                for (int page = 0; page < pages; page++) {
                    int index = page * DISPLAY_WIDTH + x;
                    uint8_t current_byte = frame[index];
                    frame[index] = (current_byte >> bit_offset) | carry;
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
    if (cursor_y+8 >= DISPLAY_HEIGHT) {
        scroll(-8);
        cursor_y -= 8;
    }
    print(text);
    cursor_y += 8;
}

void mod_Display::draw_text(int x, int y, uint32_t scale, const char* text) {
    draw_string(x, y, scale, text);
}

void mod_Display::draw_char_with_font(int x, int y, uint32_t scale, const uint8_t *font, char c) {
    if(c < font[3] || c > font[4])
        return;

    uint32_t parts_per_line = (font[0] >> 3) + ((font[0] & 7) > 0);
    for(uint8_t w = 0; w < font[1]; ++w) {
        uint32_t pp = (c - font[3]) * font[1] * parts_per_line + w * parts_per_line + 5;
        for(uint32_t lp = 0; lp < parts_per_line; ++lp) {
            uint8_t line = font[pp];
            for(int8_t j = 0; j < 8; ++j, line >>= 1) {
                if(line & 1)
                    draw_square(x + w * scale, y + ((lp << 3) + j) * scale, scale, scale);
            }
            ++pp;
        }
    }
}

void mod_Display::draw_string_with_font(int x, int y, uint32_t scale, const uint8_t *font, const char *s) {
    for(int32_t x_n = x; *s; x_n += (font[1] + font[2]) * scale) {
        draw_char_with_font(x_n, y, scale, font, *(s++));
    }
}

/*
 * Format
 * <height>, <width>, <additional spacing per char>, 
 * <first ascii char>, <last ascii char>,
 * <data>
 */
const uint8_t font_8x5[] =
{
			8, 5, 1, 32, 126,
			0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x5F, 0x00, 0x00,
			0x00, 0x07, 0x00, 0x07, 0x00,
			0x14, 0x7F, 0x14, 0x7F, 0x14,
			0x24, 0x2A, 0x7F, 0x2A, 0x12,
			0x23, 0x13, 0x08, 0x64, 0x62,
			0x36, 0x49, 0x56, 0x20, 0x50,
			0x00, 0x08, 0x07, 0x03, 0x00,
			0x00, 0x1C, 0x22, 0x41, 0x00,
			0x00, 0x41, 0x22, 0x1C, 0x00,
			0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
			0x08, 0x08, 0x3E, 0x08, 0x08,
			0x00, 0x80, 0x70, 0x30, 0x00,
			0x08, 0x08, 0x08, 0x08, 0x08,
			0x00, 0x00, 0x60, 0x60, 0x00,
			0x20, 0x10, 0x08, 0x04, 0x02,
			0x3E, 0x51, 0x49, 0x45, 0x3E,
			0x00, 0x42, 0x7F, 0x40, 0x00,
			0x72, 0x49, 0x49, 0x49, 0x46,
			0x21, 0x41, 0x49, 0x4D, 0x33,
			0x18, 0x14, 0x12, 0x7F, 0x10,
			0x27, 0x45, 0x45, 0x45, 0x39,
			0x3C, 0x4A, 0x49, 0x49, 0x31,
			0x41, 0x21, 0x11, 0x09, 0x07,
			0x36, 0x49, 0x49, 0x49, 0x36,
			0x46, 0x49, 0x49, 0x29, 0x1E,
			0x00, 0x00, 0x14, 0x00, 0x00,
			0x00, 0x40, 0x34, 0x00, 0x00,
			0x00, 0x08, 0x14, 0x22, 0x41,
			0x14, 0x14, 0x14, 0x14, 0x14,
			0x00, 0x41, 0x22, 0x14, 0x08,
			0x02, 0x01, 0x59, 0x09, 0x06,
			0x3E, 0x41, 0x5D, 0x59, 0x4E,
			0x7C, 0x12, 0x11, 0x12, 0x7C,
			0x7F, 0x49, 0x49, 0x49, 0x36,
			0x3E, 0x41, 0x41, 0x41, 0x22,
			0x7F, 0x41, 0x41, 0x41, 0x3E,
			0x7F, 0x49, 0x49, 0x49, 0x41,
			0x7F, 0x09, 0x09, 0x09, 0x01,
			0x3E, 0x41, 0x41, 0x51, 0x73,
			0x7F, 0x08, 0x08, 0x08, 0x7F,
			0x00, 0x41, 0x7F, 0x41, 0x00,
			0x20, 0x40, 0x41, 0x3F, 0x01,
			0x7F, 0x08, 0x14, 0x22, 0x41,
			0x7F, 0x40, 0x40, 0x40, 0x40,
			0x7F, 0x02, 0x1C, 0x02, 0x7F,
			0x7F, 0x04, 0x08, 0x10, 0x7F,
			0x3E, 0x41, 0x41, 0x41, 0x3E,
			0x7F, 0x09, 0x09, 0x09, 0x06,
			0x3E, 0x41, 0x51, 0x21, 0x5E,
			0x7F, 0x09, 0x19, 0x29, 0x46,
			0x26, 0x49, 0x49, 0x49, 0x32,
			0x03, 0x01, 0x7F, 0x01, 0x03,
			0x3F, 0x40, 0x40, 0x40, 0x3F,
			0x1F, 0x20, 0x40, 0x20, 0x1F,
			0x3F, 0x40, 0x38, 0x40, 0x3F,
			0x63, 0x14, 0x08, 0x14, 0x63,
			0x03, 0x04, 0x78, 0x04, 0x03,
			0x61, 0x59, 0x49, 0x4D, 0x43,
			0x00, 0x7F, 0x41, 0x41, 0x41,
			0x02, 0x04, 0x08, 0x10, 0x20,
			0x00, 0x41, 0x41, 0x41, 0x7F,
			0x04, 0x02, 0x01, 0x02, 0x04,
			0x40, 0x40, 0x40, 0x40, 0x40,
			0x00, 0x03, 0x07, 0x08, 0x00,
			0x20, 0x54, 0x54, 0x78, 0x40,
			0x7F, 0x28, 0x44, 0x44, 0x38,
			0x38, 0x44, 0x44, 0x44, 0x28,
			0x38, 0x44, 0x44, 0x28, 0x7F,
			0x38, 0x54, 0x54, 0x54, 0x18,
			0x00, 0x08, 0x7E, 0x09, 0x02,
			0x18, 0xA4, 0xA4, 0x9C, 0x78,
			0x7F, 0x08, 0x04, 0x04, 0x78,
			0x00, 0x44, 0x7D, 0x40, 0x00,
			0x20, 0x40, 0x40, 0x3D, 0x00,
			0x7F, 0x10, 0x28, 0x44, 0x00,
			0x00, 0x41, 0x7F, 0x40, 0x00,
			0x7C, 0x04, 0x78, 0x04, 0x78,
			0x7C, 0x08, 0x04, 0x04, 0x78,
			0x38, 0x44, 0x44, 0x44, 0x38,
			0xFC, 0x18, 0x24, 0x24, 0x18,
			0x18, 0x24, 0x24, 0x18, 0xFC,
			0x7C, 0x08, 0x04, 0x04, 0x08,
			0x48, 0x54, 0x54, 0x54, 0x24,
			0x04, 0x04, 0x3F, 0x44, 0x24,
			0x3C, 0x40, 0x40, 0x20, 0x7C,
			0x1C, 0x20, 0x40, 0x20, 0x1C,
			0x3C, 0x40, 0x30, 0x40, 0x3C,
			0x44, 0x28, 0x10, 0x28, 0x44,
			0x4C, 0x90, 0x90, 0x90, 0x7C,
			0x44, 0x64, 0x54, 0x4C, 0x44,
			0x00, 0x08, 0x36, 0x41, 0x00,
			0x00, 0x00, 0x77, 0x00, 0x00,
			0x00, 0x41, 0x36, 0x08, 0x00,
			0x02, 0x01, 0x02, 0x04, 0x02,
};

void mod_Display::draw_char(int x, int y, uint32_t scale, char c) {
    draw_char_with_font(x, y, scale, font_8x5, c);
}

void mod_Display::draw_string(int x, int y, uint32_t scale, const char *s) {
    draw_string_with_font(x, y, scale, font_8x5, s);
}
