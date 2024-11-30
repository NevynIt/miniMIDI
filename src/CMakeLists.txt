# Add executable. Default name is the project name, version 0.1

add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)
# add_subdirectory(iir1 iir1_build)

# Define the parameter FPM_WIDTH
set(FPM_WIDTH 16)

# Generate Table_sinWave.cpp dynamically
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/dsp/Tables_DSP.cpp
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/dsp/Tables_DSP.h
    COMMAND ${CMAKE_COMMAND} -E env python ${CMAKE_CURRENT_LIST_DIR}/dsp/genTables.py ${FPM_WIDTH}
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/dsp/genTables.py
)

add_custom_target(generate_tables DEPENDS ${CMAKE_CURRENT_LIST_DIR}/dsp/Tables_DSP.cpp)

add_executable(miniMIDI 
    miniMIDI.cpp
    App.cpp
    module.cpp
    message.cpp
    queue.cpp
    msg_MIDI.cpp
    mod_UART.cpp
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
    mod_USB_MSC.cpp
    mod_I2S_Audio.cpp
    mod_DSP.cpp
    mod_Blink.cpp
    mod_Synth.cpp
    mod_Stdio.cpp
    embedded-cli/lib/src/embedded_cli.c

    # wavetables/WT_BASE.cpp
    dsp/Tables_DSP.cpp
    dsp/iir.cpp
    dsp/wave.cpp
    dsp/exec_asm.cpp
    midi_const/midi_frequencies.cpp
    ledColors/led_colors.cpp
    # QuadratureDecoder/QuadratureDecoder.cpp
    rp2040_i2s_example/i2s.c
    # pico-ssd1306/ssd1306.c
    RP2040-pico_synth_ex/pico_synth_ex.c
    RP2040-pico_synth_ex/sound_i2s/sound_i2s.c
    # i2s_example.c
)

add_dependencies(miniMIDI generate_tables)

# Add lua static library
add_library(lua STATIC
    lua/lapi.c
    lua/lauxlib.c
    lua/lbaselib.c
    lua/lcode.c
    lua/lcorolib.c
    lua/lctype.c
    lua/ldblib.c
    lua/ldebug.c
    lua/ldo.c
    lua/ldump.c
    lua/lfunc.c
    lua/lgc.c
    lua/linit.c
    lua/liolib.c
    lua/llex.c
    lua/lmathlib.c
    lua/lmem.c
    lua/loadlib.c
    lua/lobject.c
    lua/lopcodes.c
    lua/loslib.c
    lua/lparser.c
    lua/lstate.c
    lua/lstring.c
    lua/lstrlib.c
    lua/ltable.c
    lua/ltablib.c
    lua/ltm.c
    lua/lua.c
    lua/luac.c
    lua/lundump.c
    lua/lutf8lib.c
    lua/lvm.c
    lua/lzio.c
)

# Add include directory for lua
target_include_directories(lua PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lua)

# Link lua static library to miniMIDI
target_link_libraries(miniMIDI lua)

# Add this line to set the compile option to disable the warning about the parameter passing convention for fp_int
# target_compile_options(miniMIDI PRIVATE -Wno-psabi)
# target_compile_options(iir PRIVATE -Wno-psabi)

# Set the output directories to the root build directory
set_target_properties(miniMIDI PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

pico_set_program_name(miniMIDI "miniMIDI")
pico_set_program_version(miniMIDI "0.1")

# Generate PIO headers
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
# pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/QuadratureDecoder/QuadratureDecoder.pio)
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/rp2040_i2s_example/i2s.pio)
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/RP2040-pico_synth_ex/sound_i2s/sound_i2s_16bits.pio)
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/sound_i2s_32bits_io.pio)

set (PICO_DEFAULT_UART_TX_PIN 16)
set (PICO_DEFAULT_UART_RX_PIN 17)
set (PICO_DEFAULT_UART_BAUD_RATE 115200)
set (PICO_DEFAULT_UART 0)
set(PICO_FLASH_SIZE_BYTES 16777216)
target_compile_definitions(miniMIDI PUBLIC
  PICO_DEFAULT_UART_TX_PIN=16
  PICO_DEFAULT_UART_RX_PIN=17
  PICO_DEFAULT_UART_BAUD_RATE=115200
  PICO_DEFAULT_UART=0
  # IIR1_NO_EXCEPTIONS=1
  FPM_WIDTH=${FPM_WIDTH}
  PICO_ENTER_USB_BOOT_ON_EXIT=1
  LUA_32BITS=1
)

# set (_GLIBCXX_USE_C99_COMPLEX 0)
# target_compile_definitions(iir PUBLIC
# _GLIBCXX_USE_C99_COMPLEX=0
# )

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_usb(miniMIDI 0)
pico_enable_stdio_uart(miniMIDI 1)

# Add the standard include files to the build
target_include_directories(miniMIDI PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
  ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI/include
  ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI/sd_driver
  # ${CMAKE_CURRENT_LIST_DIR}/pico-ssd1306
  # ${CMAKE_CURRENT_LIST_DIR}/QuadratureDecoder
  ${CMAKE_CURRENT_LIST_DIR}/ledColors
  # ${CMAKE_CURRENT_LIST_DIR}/wavetables
  ${CMAKE_CURRENT_LIST_DIR}/midi_const
  # ${CMAKE_CURRENT_LIST_DIR}/fpm/include
  ${CMAKE_CURRENT_LIST_DIR}/rp2040_i2s_example
  ${CMAKE_CURRENT_LIST_DIR}/RP2040-pico_synth_ex
  ${CMAKE_CURRENT_LIST_DIR}/RP2040-pico_synth_ex/sound_i2s
  ${CMAKE_CURRENT_LIST_DIR}/embedded-cli/lib/include
)

# Add any user requested libraries
target_link_libraries(miniMIDI 
        stdc++
        pico_stdlib
        pico_multicore
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_adc
        hardware_pio
        hardware_pwm
        hardware_flash
        FatFs_SPI
        tinyusb_device
        tinyusb_board
        lua
        # iir
)

pico_add_extra_outputs(miniMIDI)