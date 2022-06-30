add_executable(blink
        main.c
        matrix.c
        gif.c
        )

# pull in common dependencies
target_link_libraries(blink
        pico_stdlib
        pico_multicore
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(blink)

# add url via pico_set_program_url
example_auto_set_url(blink)
