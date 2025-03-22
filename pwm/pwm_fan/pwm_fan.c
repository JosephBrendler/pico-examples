    #include <stdio.h>
    #include <stdlib.h>
    #include "pico/stdlib.h"
    #include "hardware/uart.h"
    #include "hardware/pwm.h"

    #define UART_ID uart0
    #define BAUD_RATE 115200
    #define UART_TX_PIN 0
    #define UART_RX_PIN 1
//    #define FAN_PWM_PIN 28     already using 28 for LED; use GPIO26 instead
    #define FAN_PWM_PIN 26
//    #define TEMP_THRESHOLD 60.0    set lower instead
    #define TEMP_THRESHOLD 45.0

    void set_pwm_duty_cycle(uint slice_num, uint channel, float duty_cycle) {
        if (duty_cycle < 0.0f) duty_cycle = 0.0f;
        if (duty_cycle > 100.0f) duty_cycle = 100.0f;
        uint16_t level = (uint16_t)(duty_cycle * (float)(1 << 16) / 100.0f);
        pwm_set_gpio_level(FAN_PWM_PIN, level);
    }

    int main() {
        stdio_init_all();

        uart_init(UART_ID, BAUD_RATE);
        gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

        uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
        uart_set_fifo_enabled(UART_ID, false);

        gpio_set_function(FAN_PWM_PIN, GPIO_FUNC_PWM);
        uint slice_num = pwm_gpio_to_slice_num(FAN_PWM_PIN);
        pwm_config config = pwm_get_default_config();
        pwm_config_set_clkdiv(&config, 4.0f);
        pwm_init(slice_num, &config, true);

        char buffer[32];
        int index = 0;

        printf("Waiting for data...\n");

        while (1) {
            if (uart_is_readable(UART_ID)) {
                char c = uart_getc(UART_ID);
                if (c == '\n') {
                    buffer[index] = '\0';
                    float temperature = atof(buffer);
                    printf("Received temperature: %.2f°C\n", temperature);
                    if (temperature > TEMP_THRESHOLD) {
                        set_pwm_duty_cycle(slice_num, PWM_CHAN_A, 100.0f);
                    } else {
                        set_pwm_duty_cycle(slice_num, PWM_CHAN_A, 0.0f);
                    }
                    index = 0;
                } else {
                    buffer[index++] = c;
                    if (index >= sizeof(buffer)) {
                        index = sizeof(buffer) - 1;
                    }
                }
            }
        }

        return 0;
    }
