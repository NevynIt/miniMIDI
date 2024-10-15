add_executable(tusb_midi
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

target_link_libraries(tusb_midi PUBLIC
        pico_stdlib pico_unique_id tinyusb_device tinyusb_board
        )

target_include_directories(tusb_midi PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# create map/bin/hex file etc.
pico_add_extra_outputs(tusb_midi)

# add url via pico_set_program_url
example_auto_set_url(tusb_midi)
