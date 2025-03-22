#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define IRQ_PIN 15

volatile uint64_t lastInterruptTime = 0;
volatile uint64_t timeBetweenInterrupts = 0;
volatile uint64_t currentTime = 0;

void gpio_callback(uint gpio, uint32_t events) {
    currentTime = time_us_64();
    timeBetweenInterrupts = currentTime - lastInterruptTime;
    lastInterruptTime = currentTime;
}

int main() {
    stdio_init_all();

    gpio_init(IRQ_PIN);
    gpio_set_dir(IRQ_PIN, GPIO_IN);
    gpio_pull_down(IRQ_PIN);
    gpio_set_irq_enabled_with_callback(IRQ_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    while (1) {
        printf("Time between interrupts: %llu microseconds\n", timeBetweenInterrupts);
        sleep_ms(1000);
    }
}
