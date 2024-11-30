#ifndef HWCONFIG_H
#define HWCONFIG_H

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/spi.h"
#include <stdint.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define DEOPTIMIZE \
    _Pragma(TOSTRING(message("Deoptimizing here"))) \
    __attribute__((optimize("O0")))

static constexpr uint16_t compiled_hw_cfg_version = 1; //update this whenever a breaking change is done in the struct below

//slowly update all modules to use this instead of the defines
struct hw_cfg
{
public:
    uint16_t version = compiled_hw_cfg_version;

    //system configuration
    uint32_t cpu_khz = 12*12*1000;
    static constexpr uint32_t flash_reserved = 4*1024*1024; //keep hardcoded

    //display configuration
    static constexpr uint8_t display_addr = 0x3C; //keep hardcoded
    static constexpr uint8_t display_width = 128; //keep hardcoded
    static constexpr uint8_t display_height = 64; //keep hardcoded
    static constexpr uint16_t display_bufsize = display_width*display_height/8; //keep hardcoded
    uint8_t i2c_display = 1;
    uint8_t display_refresh_ms = 100;
    
    //onboard led configuration
    uint16_t blink_refresh_ms = 1000/24;

    //joystick configuration
    uint8_t adc_joys_x = 2;
    uint8_t adc_joys_y = 3;

    //encoder configuration
    uint8_t encoder_count = 4;

    //SD configuration
    uint32_t spi_baud_rate = 125 * 1000 * 1000 / 4;  // 31250000 Hz

    //uart configuration
    uint32_t uart_baudrate = 115200;

    //rgb led configuration
    uint16_t ledstrip_count = 23;
    uint16_t ledstrip_refresh_ms = 1000/25;
    uint8_t ledstrip_pio = 1;

    //audio dsp configuration
    static constexpr uint16_t dsp_sample_rate = 48000; //keep hardcoded
    static constexpr uint8_t dsp_sample_width = 16; //keep hardcoded
    static constexpr uint8_t dsp_buffer_ms = 1; //keep hardcoded
    static constexpr uint8_t dsp_buffer_samples = dsp_sample_rate / 1000 * dsp_buffer_ms; //keep hardcoded
    static constexpr uint16_t dsp_buffer_size = dsp_buffer_samples * (dsp_sample_width / 8); //keep hardcoded
    uint8_t dsp_buffer_slots = 8;
    uint8_t dsp_buffer_tracks = 10;

    //i2s configuration
    uint8_t i2s_pio = 1;
    static constexpr uint8_t i2s_bits_per_channel = 32; //keep hardcoded
    static constexpr uint8_t i2s_channels = 2; //keep hardcoded
    static constexpr uint16_t i2s_buffer_size = dsp_buffer_samples * i2s_bits_per_channel / 8 * i2s_channels; //keep hardcoded
    
    //analog audio configuration
    //TODO: add pwm and adc based audio interfaces

    //gpio pins
    uint8_t gpio_encoder_base = 0;
    uint8_t gpio_sd_rx = 12;
    uint8_t gpio_sd_cs = 13;
    uint8_t gpio_sd_ck = 14;
    uint8_t gpio_sd_tx = 15;
    uint8_t gpio_uart_tx = 16;
    uint8_t gpio_uart_rx = 17;
    uint8_t gpio_audio_ck = 18;
    uint8_t gpio_audio_ws = 19;
    uint8_t gpio_audio_do = 20;
    uint8_t gpio_audio_di = 21;
    uint8_t gpio_ledstrip = 22;
    uint8_t gpio_joys_btn = 23;
    uint8_t gpio_blink_led = 25;
    uint8_t gpio_display_sda = 26;
    uint8_t gpio_display_scl = 27;
    uint8_t gpio_joys_x = 28;
    uint8_t gpio_joys_y = 29;

    //board specific pins
    uint8_t gpio_board_btn = 24;
    uint8_t gpio_board_led = 25;
    uint8_t gpio_board_rgb = 23;
};

typedef enum {
    GPIO_Enc0_A = 0,    // 0
    GPIO_Enc0_B,        // 1
    GPIO_Enc1_A,        // 2
    GPIO_Enc1_B,        // 3
    GPIO_Enc2_A,        // 4
    GPIO_Enc2_B,        // 5
    GPIO_Enc3_A,        // 6
    GPIO_Enc3_B,        // 7
    GPIO_Enc0_Btn,      // 8
    GPIO_Enc1_Btn,      // 9
    GPIO_Enc2_Btn,      // 10
    GPIO_Enc3_Btn,      // 11
    GPIO_SD_RX,         // 12 - SPI1 RX
    GPIO_SD_CS,         // 13 - SPI1 CS
    GPIO_SD_CK,         // 14 - SPI1 CK
    GPIO_SD_TX,         // 15 - SPI1 TX
    GPIO_UART_TX,       // 16 - UART0 TX make sure to update the GPIO pins in CMakeLists.txt to use as STDIO
    GPIO_UART_RX,       // 17 - UART0 RX
    GPIO_Audio_CK,      // 18 - I2S CK
    GPIO_Audio_WS,      // 19 - I2S WS
    GPIO_Audio_DO,      // 20 - I2S DO
    GPIO_Audio_DI,      // 21 - I2S DI
    GPIO_LedStrip,      // 22 - WS2812B
    GPIO_Joys_Btn,      // 23 - (Board RGB Led)
    GPIO_Board_Btn,     // 24 - (Board Button)
    GPIO_Board_Led,     // 25 - (Board Blue Led)
    GPIO_Display_SDA,   // 26 - I2C1 SDA
    GPIO_Display_SCL,   // 27 - I2C1 SCL
    GPIO_Joys_X,        // 28 - ADC 2
    GPIO_Joys_Y,        // 29 - ADC 3
    GPIO_Board_RGB = 23 // 23 - (Board RGB Led)
} GPIO_Pin;

struct UartPinMapping {
    int tx_pin;
    int rx_pin;
    uart_inst_t *uart_instance;
};

static const UartPinMapping uart_pin_map[] = { //I have included only consecutive pins for now
    {0, 1, uart0},
    {4, 5, uart1},
    {8, 9, uart1},
    {12, 13, uart0},
    {16, 17, uart0},
    {20, 21, uart1},
    {24, 25, uart1},
    {28, 29, uart0}
};

inline uart_inst_t* get_uart_instance(int tx_pin, int rx_pin) {
    for (const auto& mapping : uart_pin_map) {
        if (mapping.tx_pin == tx_pin && mapping.rx_pin == rx_pin) {
            return mapping.uart_instance;
        }
    }
    return nullptr;
}

struct SpiPinMapping {
    int rx_pin;
    int ck_pin;
    int tx_pin;
    spi_inst_t *spi_instance;
};

static const SpiPinMapping spi_pin_map[] = { //I have included only consecutive pins for now
    {0, 2, 3, spi0},
    {4, 2, 3, spi0},
    {4, 6, 7, spi0},
    {8, 10, 11, spi1},
    {12, 10, 11, spi1},
    {12, 14, 15, spi1},
    {16, 18, 19, spi0},
    {20, 18, 19, spi0},
    {20, 22, 23, spi0},
    {24, 26, 27, spi1},
    // Add more mappings as needed
};

inline spi_inst_t* get_spi_instance(int rx_pin, int ck_pin, int tx_pin) {
    for (const auto& mapping : spi_pin_map) {
        if (mapping.rx_pin == rx_pin && mapping.ck_pin == ck_pin && mapping.tx_pin == tx_pin) {
            return mapping.spi_instance;
        }
    }
    return nullptr;
}

// Flash Configuration
#define FLASH_RESERVED 4*1024*1024 //4MB reserved for code, the rest for data

// Display configuration
#define I2C_DISPLAY i2c1
#define DISPLAY_ADDR 0x3C
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DISPLAY_BUFSIZE DISPLAY_WIDTH * DISPLAY_HEIGHT / 8
#define DISPLAY_REFRESH_MS 100

// Joystick configuration
#define ADC_Joys_X 2
#define ADC_Joys_Y 3

// Encoder configuration
#define NUM_ENCODERS 4
#define GPIO_Enc_Base GPIO_Enc0_A

// RGB LED configuration
#define PIO_WS2812B  pio1
#define LEDS_COUNT 23 // For testing purposes, the actual number of LEDs for the piano is 169
#define LEDS_REFRESH_MS (1000/25)
//pixel map defined in mod_LedStrip.cpp

// Audio DSP configuration
#define SAMPLE_RATE 48000 // use multiples of 1000
#define SAMPLE_TYPE int16_t  // 16 bits per sample
typedef SAMPLE_TYPE sample_t;
#define BITS_PER_SAMPLE (sizeof(SAMPLE_TYPE) * 8)
#define SAMPLE_MAX INT16_MAX
#define SAMPLE_MIN INT16_MIN
#define SAMPLE_ZERO 0
typedef sample_t* sample_ptr;
typedef const sample_t* sample_cptr;
#define AUDIO_BUFFER_MS 1 //1ms to be consistent with the USB audio buffer
#define AUDIO_BUFFER_US (AUDIO_BUFFER_MS * 1000)
#define AUDIO_BUFFER_SAMPLES (SAMPLE_RATE / 1000 * AUDIO_BUFFER_MS)
#define AUDIO_BUFFER_SIZE AUDIO_BUFFER_SAMPLES * (BITS_PER_SAMPLE / 8)
#define AUDIO_BUFFER_SLOTS 8
#define AUDIO_BUFFER_SLOTS_MASK (AUDIO_BUFFER_SLOTS - 1)
#define AUDIO_BUFFER_TRACKS 10

enum DSP_Tracks {
    DSP_TRACK_USB_IN_LEFT,
    DSP_TRACK_USB_IN_RIGHT,
    DSP_TRACK_USB_OUT_LEFT,
    DSP_TRACK_USB_OUT_RIGHT,
    DSP_TRACK_SYNTH,
    DSP_TRACK_MIC,
    DSP_TRACK_SPK,
    DSP_TRACK_DSP_BASE
};

// I2S configuration
#define PIO_I2S      pio1
#define I2S_BITS_PER_CHANNEL 32 //hard coded in the pio program
#define I2S_CHANNELS 2 //hard coded in the pio program
#define I2S_BUFFER_SAMPLES AUDIO_BUFFER_SAMPLES
#define I2S_BUFFER_NUM 2
#define I2S_BUFFER_SIZE (I2S_BUFFER_SAMPLES * I2S_CHANNELS * I2S_BITS_PER_CHANNEL / 8)

// UART configuration - LIB_PICO_STDIO_UART defined implicitly in CMakeLists.txt
#define UART_INST uart0  //deconflicting the name with the standard macro
#if LIB_PICO_STDIO_UART
    #define UART_BAUD_RATE 115200
#else
    #define UART_BAUD_RATE 31250
#endif

//USB configuration mostly defined in mod_USB.cpp

#endif // HWCONFIG_H