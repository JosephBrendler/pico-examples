add_executable(interrupt
    interrupt.c
    )

# pull in common dependencies
target_link_libraries(interrupt pico_stdlib)
# pull in pico.stdlib which includes commonly used features (plus gpio, adc, pwm)
target_link_libraries(interrupt pico_stdlib hardware_gpio hardware_timer)

# enable usb output, disable uart output
pico_enable_stdio_usb(interrupt 1)
pico_enable_stdio_uart(interrupt 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(interrupt)

# add url via pico_set_program_url
example_auto_set_url(interrupt)
