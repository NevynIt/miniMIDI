#ifndef HWCONFIG_H
#define HWCONFIG_H

#include "hardware/gpio.h"

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
    GPIO_MIDI_TX,       // 16 - UART0 TX
    GPIO_MIDI_RX,       // 17 - UART0 RX
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

// SD card configuration
#define SPI_SD spi1

// Display configuration
#define I2C_DISPLAY i2c1
#define DISPLAY_ADDR 0x3C
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
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

// Audio configuration
#define PIO_I2S      pio1
#define SAMPLE_RATE 48000
#define BITS_PER_SAMPLE 16
#define I2S_BITS_PER_CHANNEL 32 //hard coded in the pio program
#define I2S_CHANNELS 2 //hard coded in the pio program
#define I2S_BUFFER_SAMPLES SAMPLE_RATE/1000
#define I2S_BUFFER_NUM 2
#define I2S_BUFFER_SIZE I2S_BUFFER_SAMPLES * I2S_CHANNELS * I2S_BITS_PER_CHANNEL / 8

// MIDI configuration
#define UART_MIDI uart0
//#define MIDI_BAUD_RATE 31250
#define MIDI_BAUD_RATE 115200 //for testing purposes, emulating midi interaction on the debugger

#endif // HWCONFIG_H