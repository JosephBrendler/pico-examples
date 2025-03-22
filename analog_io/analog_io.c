/**
 * Copyright (c) 2025-2055 joe brendler
 *
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define POT_PIN 26      // A0 = GPIO 26
#define LED_PWM_PIN 25  // GPIO 25

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min ;
}

int main() {
  stdio_init_all();

  // initialize adc
  adc_init();
  // Make sure GPIO is high-impedance, no pullups etc
  adc_gpio_init(POT_PIN);
  // Select ADC input 0 (GPIO26)
  adc_select_input(0);
  printf("ADC Example, measuring potentiometer on GPIO %d\n", POT_PIN);

  // initialize pwm
  // Tell GPIO LED_PWM_PIN it is allocated to the PWM
  gpio_set_function(LED_PWM_PIN, GPIO_FUNC_PWM);
  // Find out which PWM slice is connected to LED_PWM_PIN (GPIO25 should be 4)
  uint slice_num = pwm_gpio_to_slice_num(LED_PWM_PIN);
  // Set period of 256 cycles (0 to 255 inclusive)
  pwm_set_wrap(slice_num, 255);
  // GPIO 25 is on PWM_CHAN_B, so set channel B output low to start
  pwm_set_chan_level(slice_num, PWM_CHAN_B, 0);
  // Enable the PWM and set it running
  pwm_set_enabled(slice_num, true);

  while (1) {
    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / (1 << 12);
    uint16_t result = adc_read();
    long pwm_value = map(result, 0, 4095, 0, 255);

    printf("Raw value: %d \t voltage: %f V \t PWM: %d\n", result, result * conversion_factor, pwm_value);

    pwm_set_chan_level(slice_num, PWM_CHAN_B, pwm_value);
    sleep_ms(50);
  }
}
