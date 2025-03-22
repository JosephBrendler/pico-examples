add_executable(pwm_status_with_analog_io
    pwm_status_with_analog_io.c
    )

# pull in pico.stdlib which includes commonly used features (plus gpio, adc, pwm)
target_link_libraries(
    pwm_status_with_analog_io
    pico_stdlib
    hardware_uart
    hardware_gpio
    hardware_adc
    hardware_pwm
    hardware_irq
    )

# enable/disable usb or uart output as needed by program
#pico_enable_stdio_usb(pwm_status_with_analog_io 1)
#pico_enable_stdio_uart(pwm_status_with_analog_io 0)
pico_enable_stdio_usb(pwm_status_with_analog_io 0)
pico_enable_stdio_uart(pwm_status_with_analog_io 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(pwm_status_with_analog_io)

# add url via pico_set_program_url
example_auto_set_url(pwm_status_with_analog_io)
