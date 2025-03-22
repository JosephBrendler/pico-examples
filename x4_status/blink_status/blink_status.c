#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define GREEN_LED 28  // GPIO28 (pin 3)
#define YELLOW_LED 29 // GPIO29 (pin 5)
#define RED_LED 4     // GPIO04 (pin 7)
                      // use pin 9 as GND

// define status bits with which to read input from serial
#define GBIT 0
#define YBIT 1
#define RBIT 2

int main() {
    // initialize LEDs
    gpio_init(GREEN_LED);
    gpio_init(YELLOW_LED);
    gpio_init(RED_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);
    gpio_set_dir(RED_LED, GPIO_OUT);

    // initialize UART
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, false);

    // initialize variables with which to read input from serial
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
                printf("Received status: %d\n", status);
                // act on received status (green)
                if (status & (1 << GBIT)) {
                    gpio_put(GREEN_LED, 1);
                } else {
                    gpio_put(GREEN_LED, 0);
                }
                // act on received status (yellow)
                if (status & (1 << YBIT)) {
                    gpio_put(YELLOW_LED, 1);
                } else {
                    gpio_put(YELLOW_LED, 0);
                }
                // act on received status (green)
                if (status & (1 << RBIT)) {
                    gpio_put(RED_LED, 1);
                } else {
                    gpio_put(RED_LED, 0);
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
