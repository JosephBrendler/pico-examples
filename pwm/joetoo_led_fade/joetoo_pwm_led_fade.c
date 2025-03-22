#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

//#define JOE_g_fade_LED_PIN 26
#define GREEN_LED_PIN 28    // green LED on joetoo LED header
#define YELLOW_LED_PIN 29    // green LED on joetoo LED header
#define RED_LED_PIN 4    // green LED on joetoo LED header

void on_pwm_wrap() {
    static int fade = 0;
    static int g_fade = 0;
    static int y_fade = 0;
    static int r_fade = 0;
    static bool going_up = true;

    pwm_clear_irq(pwm_gpio_to_slice_num(GREEN_LED_PIN));
    pwm_clear_irq(pwm_gpio_to_slice_num(YELLOW_LED_PIN));
    pwm_clear_irq(pwm_gpio_to_slice_num(RED_LED_PIN));

    if (going_up) {
        ++fade;
        if (fade > 255) {
            fade = 255;
            going_up = false;
        }
    } else {
        --fade;
        if (fade < 0) {
            fade = 0;
            going_up = true;
        }
    }
    // keep y and r pwm signals at a linear offset (modularly wrapped) from g
    g_fade = fade;
    y_fade = ( fade + 85 ) % 256;
    r_fade = ( fade + 170 ) % 256;

    pwm_set_gpio_level(GREEN_LED_PIN, g_fade * g_fade);
    pwm_set_gpio_level(YELLOW_LED_PIN, y_fade * y_fade);
    pwm_set_gpio_level(RED_LED_PIN, r_fade * r_fade);
}

int main() {

    gpio_set_function(GREEN_LED_PIN, GPIO_FUNC_PWM);
    gpio_set_function(YELLOW_LED_PIN, GPIO_FUNC_PWM);
    gpio_set_function(RED_LED_PIN, GPIO_FUNC_PWM);

    uint gy_slice_num = pwm_gpio_to_slice_num(GREEN_LED_PIN);
    uint r_slice_num = pwm_gpio_to_slice_num(RED_LED_PIN);

    pwm_clear_irq(gy_slice_num);
    pwm_clear_irq(r_slice_num);
    pwm_set_irq_enabled(gy_slice_num, true);
    pwm_set_irq_enabled(r_slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config config = pwm_get_default_config();
//    pwm_config_set_clkdiv(&config, 4.f);   // lets go slower
    pwm_config_set_clkdiv(&config, 16.f);
    pwm_init(gy_slice_num, &config, true);
    pwm_init(r_slice_num, &config, true);

    while (1)
        tight_loop_contents();
}

