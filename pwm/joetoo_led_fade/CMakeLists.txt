add_executable(joetoo_pwm_led_fade
        joetoo_pwm_led_fade.c
        )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(joetoo_pwm_led_fade pico_stdlib hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(joetoo_pwm_led_fade)

# add url via pico_set_program_url
example_auto_set_url(joetoo_pwm_led_fade)
