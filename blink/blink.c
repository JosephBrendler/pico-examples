#include "pico/stdlib.h"
#define GREEN_LED 28  // GPIO28 (pin 3)
#define YELLOW_LED 29 // GPIO29 (pin 5)
#define RED_LED 4     // GPIO04 (pin 7)
                      // use pin 9 as GND
#define T_WAIT 500
int main() {

    gpio_init(GREEN_LED);
    gpio_init(YELLOW_LED);
    gpio_init(RED_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);
    gpio_set_dir(RED_LED, GPIO_OUT);

    while (true) {
        gpio_put(GREEN_LED, 1);
        sleep_ms(T_WAIT);
        gpio_put(YELLOW_LED, 1);
        sleep_ms(T_WAIT);
        gpio_put(RED_LED, 1);
        sleep_ms(T_WAIT);
        gpio_put(GREEN_LED, 0);
        sleep_ms(T_WAIT);
        gpio_put(YELLOW_LED, 0);
        sleep_ms(T_WAIT);
        gpio_put(RED_LED, 0);
        sleep_ms(T_WAIT);
    }
}
