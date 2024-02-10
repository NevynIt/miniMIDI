add_executable(flash_uti
        flash_uti.c
        )

target_link_libraries(flash_uti
        pico_stdlib
        hardware_flash
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(flash_uti)

# add url via pico_set_program_url
example_auto_set_url(flash_uti)
