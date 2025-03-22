    #include <stdio.h>
    #include <stdlib.h>
    #include "pico/stdlib.h"
    #include "hardware/uart.h"
    #include "hardware/pwm.h"

    #define UART_ID uart0
    #define BAUD_RATE 115200
    #define UART_TX_PIN 0
    #define UART_RX_PIN 1

    #define GREEN_LED_PIN 28
    #define YELLOW_LED_PIN 29
    #define RED_LED_PIN 4

    // define status bits
    #define GBIT 0
    #define YBIT 1
    #define RBIT 2

    void set_green_pwm_duty_cycle(uint slice_num, uint channel, float duty_cycle) {
        if (duty_cycle < 0.0f) duty_cycle = 0.0f;
        if (duty_cycle > 100.0f) duty_cycle = 100.0f;
        uint16_t level = (uint16_t)(duty_cycle * (float)(1 << 16) / 100.0f);
        pwm_set_gpio_level(GREEN_LED_PIN, level);
    }

    void set_yellow_pwm_duty_cycle(uint slice_num, uint channel, float duty_cycle) {
        if (duty_cycle < 0.0f) duty_cycle = 0.0f;
        if (duty_cycle > 100.0f) duty_cycle = 100.0f;
        uint16_t level = (uint16_t)(duty_cycle * (float)(1 << 16) / 100.0f);
        pwm_set_gpio_level(YELLOW_LED_PIN, level);
    }

    void set_red_pwm_duty_cycle(uint slice_num, uint channel, float duty_cycle) {
        if (duty_cycle < 0.0f) duty_cycle = 0.0f;
        if (duty_cycle > 100.0f) duty_cycle = 100.0f;
        uint16_t level = (uint16_t)(duty_cycle * (float)(1 << 16) / 100.0f);
        pwm_set_gpio_level(RED_LED_PIN, level);
    }

    int main() {
        stdio_init_all();

        uart_init(UART_ID, BAUD_RATE);
        gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

        uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
        uart_set_fifo_enabled(UART_ID, false);

        gpio_set_function(GREEN_LED_PIN, GPIO_FUNC_PWM);
        uint g_slice_num = pwm_gpio_to_slice_num(GREEN_LED_PIN);
        gpio_set_function(YELLOW_LED_PIN, GPIO_FUNC_PWM);
        uint y_slice_num = pwm_gpio_to_slice_num(YELLOW_LED_PIN);
        gpio_set_function(RED_LED_PIN, GPIO_FUNC_PWM);
        uint r_slice_num = pwm_gpio_to_slice_num(RED_LED_PIN);

        pwm_config config = pwm_get_default_config();
        pwm_config_set_clkdiv(&config, 4.0f);

        pwm_init(g_slice_num, &config, true);
        pwm_init(y_slice_num, &config, true);
        pwm_init(r_slice_num, &config, true);

        char buffer[32];
        int index = 0;
        int status = 0;

        printf("Waiting for data...\n");

        while (1) {
            if (uart_is_readable(UART_ID)) {
                char c = uart_getc(UART_ID);
                if (c == '\n') {
                    buffer[index] = '\0';
                    status = atoi(buffer);
                    printf("Received status: %.2f°C\n", status);
                    // act on received status (green)
                    if (status & (1 << GBIT)) {
                        set_green_pwm_duty_cycle(g_slice_num, PWM_CHAN_A, 100.0f);
                    } else {
                        set_green_pwm_duty_cycle(g_slice_num, PWM_CHAN_A, 0.0f);
                    }
                    // act on received status (yellow)
                    if (status & (1 << YBIT)) {
                        set_yellow_pwm_duty_cycle(y_slice_num, PWM_CHAN_B, 100.0f);
                    } else {
                        set_yellow_pwm_duty_cycle(y_slice_num, PWM_CHAN_B, 0.0f);
                    }
                    // act on received status (green)
                    if (status & (1 << RBIT)) {
                        set_red_pwm_duty_cycle(r_slice_num, PWM_CHAN_A, 100.0f);
                    } else {
                        set_red_pwm_duty_cycle(r_slice_num, PWM_CHAN_A, 0.0f);
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
