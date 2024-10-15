add_executable(miniMIDI
        audio.cpp
        config.cpp
        data.cpp
        midi.cpp
        midi.cpp
        miniMIDI.cpp
        process_cmd.cpp
        process_msg.cpp
        sequencer.cpp
        serial.cpp
        usb_descriptors.cpp
        )

target_link_libraries(miniMIDI PUBLIC
        pico_stdlib
        hardware_flash
        pico_stdlib pico_unique_id tinyusb_device tinyusb_board
        )

target_include_directories(miniMIDI PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# create map/bin/hex file etc.
pico_add_extra_outputs(miniMIDI)

# add url via pico_set_program_url
example_auto_set_url(miniMIDI)
