
    add_executable(pwm_status
        pwm_status.c
        )

    # pull in common dependencies and additional pwm hardware support
    target_link_libraries(pwm_status pico_stdlib hardware_pwm)

    # create map/bin/hex file etc.
    pico_add_extra_outputs(pwm_status)

    # add url via pico_set_program_url
    example_auto_set_url(pwm_status)

