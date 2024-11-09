#ifndef HWCONFIG_H
#define HWCONFIG_H

#include "hardware/gpio.h"
#include "fpm/fixed.hpp"

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

// Board Configuration
#define CPU_KHZ 12*12*1000 //12MHz multiple needed for USB
#define BLINK_MS 500

// Flash Configuration
#define FLASH_SIZE 16*1024*1024 //16MB on the current motherboard
#define FLASH_RESERVED 4*1024*1024 //4MB reserved for code, the rest for data

// SD card configuration
#define SPI_SD spi1

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
// #define ENCODERS_USE_PIO //PIO Iplementation currently broken, Non-PIO implementation is a bit slow but kind of works
#ifdef ENCODERS_USE_PIO
#define PIO_ENCODERS pio0
#endif

// RGB LED configuration
#define PIO_WS2812B  pio1
#define LEDS_COUNT 23 // For testing purposes, the actual number of LEDs for the piano is 169
#define LEDS_REFRESH_MS 100
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
#define AUDIO_BUFFER_SLOTS 32
#define AUDIO_BUFFER_SLOTS_MASK (AUDIO_BUFFER_SLOTS - 1)
#define AUDIO_BUFFER_TRACKS 10


// Fixed-point math
// typedef fpm::fixed_16_16 fp_int;
// #define FRACTION_BITS 16
typedef fpm::fixed<16,16,true> fp_int;
typedef fpm::fixed<16,16,false> fp_uint;
#define TWO_PI (fp_int::two_pi())

// typedef float fp_int;
// #define TWO_PI (2.0f * M_PI)

// template <typename B, typename I, unsigned int F, bool R>
// inline fpm::fixed<B, I, F, R> mod1(fpm::fixed<B, I, F, R> x) noexcept
// {
//     constexpr auto mask = (B(1) << F) - 1;
//     return fpm::fixed<B, I, F, R>::from_raw_value(x.raw_value() & mask);
// }

// template<typename T>
// inline T mod1(T x) noexcept
// {
//     return x - (int)x;
// }

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