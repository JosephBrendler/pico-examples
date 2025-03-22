add_executable(joe_pwm_led_fade
        joe_pwm_led_fade.c
        )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(joe_pwm_led_fade pico_stdlib hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(joe_pwm_led_fade)

# add url via pico_set_program_url
example_auto_set_url(joe_pwm_led_fade)
