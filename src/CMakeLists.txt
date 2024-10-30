# Add executable. Default name is the project name, version 0.1

add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

add_executable(miniMIDI 
    miniMIDI.cpp
    App.cpp
    module.cpp
    message.cpp
    queue.cpp
    msg_MIDI.cpp
    mod_UART_MIDI.cpp
    mod_config.cpp
    mod_SD.cpp
    mod_Display.cpp
    mod_Encoders.cpp
    mod_Joys.cpp
    mod_LedStrip.cpp
    mod_Sequencer.cpp
    mod_USB.cpp
    mod_USB_Audio.cpp
    mod_USB_MIDI.cpp
    mod_USB_UART.cpp
    mod_Audio.cpp
    mod_DSP.cpp
    mod_Synth.cpp
    QuadratureDecoder/QuadratureDecoder.cpp
    rp2040_i2s_example/i2s.c
    pico-ssd1306/ssd1306.c
    RP2040-pico_synth_ex/pico_synth_ex.c
    RP2040-pico_synth_ex/sound_i2s/sound_i2s.c
)

# Set the output directories to the root build directory
set_target_properties(miniMIDI PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

pico_set_program_name(miniMIDI "miniMIDI")
pico_set_program_version(miniMIDI "0.1")

# Generate PIO headers
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/blink.pio)
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/QuadratureDecoder/QuadratureDecoder.pio)
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/rp2040_i2s_example/i2s.pio)
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/RP2040-pico_synth_ex/sound_i2s/sound_i2s_16bits.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(miniMIDI 0)
pico_enable_stdio_usb(miniMIDI 0)

# Add the standard library to the build
target_link_libraries(miniMIDI
        pico_stdlib
        stdc++
        tinyusb_device
        tinyusb_board
)

# Add the standard include files to the build
target_include_directories(miniMIDI PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
  ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI/include
  ${CMAKE_CURRENT_LIST_DIR}/pico-ssd1306
  ${CMAKE_CURRENT_LIST_DIR}/QuadratureDecoder
  ${CMAKE_CURRENT_LIST_DIR}/rp2040_i2s_example
  ${CMAKE_CURRENT_LIST_DIR}/RP2040-pico_synth_ex
  ${CMAKE_CURRENT_LIST_DIR}/RP2040-pico_synth_ex/sound_i2s
)

# Add any user requested libraries
target_link_libraries(miniMIDI 
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_adc
        hardware_pio
        hardware_pwm
        pico_multicore
        FatFs_SPI
)

pico_add_extra_outputs(miniMIDI)
