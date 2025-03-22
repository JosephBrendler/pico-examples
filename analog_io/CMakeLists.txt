add_executable(analog_io
        analog_io.c
        )

# pull in pico.stdlib which includes commonly used features (plus gpio, adc, pwm)
target_link_libraries(analog_io pico_stdlib hardware_gpio hardware_adc hardware_pwm)

# enable usb output, disable uart output
pico_enable_stdio_usb(analog_io 1)
pico_enable_stdio_uart(analog_io 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(analog_io)

# add url via pico_set_program_url
example_auto_set_url(analog_io)
