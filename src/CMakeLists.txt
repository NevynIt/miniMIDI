# Add executable. Default name is the project name, version 0.1

add_executable(miniMIDI 
    miniMIDI.cpp
    Module.cpp
    Message.cpp
    Queue.cpp
    msg_MIDI.cpp
    mod_UART_MIDI.cpp
)

pico_set_program_name(miniMIDI "miniMIDI")
pico_set_program_version(miniMIDI "0.1")

# Generate PIO header
pico_generate_pio_header(miniMIDI ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(miniMIDI 0)
pico_enable_stdio_usb(miniMIDI 0)

# Add the standard library to the build
target_link_libraries(miniMIDI
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(miniMIDI PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(miniMIDI 
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_pio
)

pico_add_extra_outputs(miniMIDI)
