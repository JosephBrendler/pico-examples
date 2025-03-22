/****************************************
 * Copyright (c) 2025-2055 joe brendler *
 ****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

// define status bits
#define GBIT 0
#define YBIT 1
#define RBIT 2

// define UART pins and config
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// define carriage return and line feed character integer values
#define CR 13   // ascii "\r"
#define LF 10   // ascii "\n"

// define status pins -  standard joetoo headless status header
#define GREEN_LED_PIN 28   // GPIO 28 ; pin 3
#define YELLOW_LED_PIN 29  // GPIO 29 ; pin 5
#define RED_LED_PIN 4      // GPIO 4  ; pin 7

// define analog io pins
#define POT_PIN 26         // A0 = GPIO 26
#define LED_PWM_PIN 25     // GPIO 25

// select LED PWM intensity levels (0.0f - 100.0f) for ON state
//GZ=100.0f
const float GZ = 10.0f;
const float YZ = 100.0f;
const float RZ = 100.0f;

// initialize global variables for serial communication
volatile char status = 0;
// initialize global variables for program flow control
volatile bool STATUS_CHANGED = false;
// initialize global variables for analog io
const float conversion_factor = 3.3f / (1 << 12);

// function to linearly map value to another range
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min ;
}

// function to set green LED
void set_green_pwm_duty_cycle(uint slice_num, uint channel, float duty_cycle) {
  if (duty_cycle < 0.0f) duty_cycle = 0.0f;
  if (duty_cycle > 100.0f) duty_cycle = 100.0f;
  uint16_t level = (uint16_t)(duty_cycle * (float)(1 << 16) / 100.0f);
  pwm_set_gpio_level(GREEN_LED_PIN, level);
}

// function to set yellow LED
void set_yellow_pwm_duty_cycle(uint slice_num, uint channel, float duty_cycle) {
  if (duty_cycle < 0.0f) duty_cycle = 0.0f;
  if (duty_cycle > 100.0f) duty_cycle = 100.0f;
  uint16_t level = (uint16_t)(duty_cycle * (float)(1 << 16) / 100.0f);
  pwm_set_gpio_level(YELLOW_LED_PIN, level);
}

// function to set red LED
void set_red_pwm_duty_cycle(uint slice_num, uint channel, float duty_cycle) {
  if (duty_cycle < 0.0f) duty_cycle = 0.0f;
  if (duty_cycle > 100.0f) duty_cycle = 100.0f;
  uint16_t level = (uint16_t)(duty_cycle * (float)(1 << 16) / 100.0f);
  pwm_set_gpio_level(RED_LED_PIN, level);
}

// function for RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
//        uint8_t ch = uart_getc(UART_ID);
        char ch = uart_getc(UART_ID);
        // this should be one char, but may be a string  - char followed by CR/LF
        if ( ch != CR && ch != LF ) { status = ch - '0'; }
        // you can also prevent by sending data with bash echo -n
        STATUS_CHANGED = true;
    }
}


int main() {
  stdio_init_all();

  // initialize adc
  adc_init();
  // Make sure GPIO is high-impedance, no pullups etc
  adc_gpio_init(POT_PIN);
  // Select ADC input 0 (GPIO26)
  adc_select_input(0);
  printf("ADC: measuring potentiometer on GPIO %d\n", POT_PIN);

  // initialize UART at basic baud rate and change afterward
  uart_init(UART_ID, 2400);

  // Set the TX and RX pins by using the function select on the GPIO
  gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
  gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));

  // Set a different speed - The call will return the actual baud rate selected,
  //   which will be as close as possible to that requested
  int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

  // Set UART flow control CTS/RTS, we don't want these, so turn them off
  uart_set_hw_flow(UART_ID, false, false);

  // Set our data format
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

  // Turn off FIFO's - we want to do this character by character
  uart_set_fifo_enabled(UART_ID, false);

  // Set up a RX interrupt
  // We need to set up the handler first
  // Select correct interrupt for the UART we are using
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

  // And set up and enable the interrupt handlers
  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);

  // Now enable the UART to send interrupts - RX only
  uart_set_irq_enables(UART_ID, true, false);

  // OK, all set up.
  // Lets send a basic string out, and then run a loop and wait for RX interrupts
  // The handler will count them, but also reflect the incoming data back with a slight change!
  uart_puts(UART_ID, "\nRunning pwm_status_with_analog_io\r\n");

  printf("UART ID: %d \tTX: %d \tRX: %d \tBAUD: %d N81\n", UART_ID, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);

  // set clock divider for pwm
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 4.0f);

  // initialize pwm for analog io
  // Tell GPIO LED_PWM_PIN it is allocated to the PWM
  gpio_set_function(LED_PWM_PIN, GPIO_FUNC_PWM);
  // Find out which PWM slice is connected to LED_PWM_PIN
  uint a_slice_num = pwm_gpio_to_slice_num(LED_PWM_PIN);      // should be 6 A
  // Set period of 256 cycles (0 to 255 inclusive)
  pwm_set_wrap(a_slice_num, 255);
  // GPIO 25 is on PWM_CHAN_B, so set channel B output low to start
  pwm_set_chan_level(a_slice_num, PWM_CHAN_B, 0);
  // Enable the PWM and set it running
  pwm_set_enabled(a_slice_num, true);
  printf("Analog output pwm LED pin set up on GPIO %d\n", LED_PWM_PIN);

  // similarly initialize pwm for status LEDs
  gpio_set_function(GREEN_LED_PIN, GPIO_FUNC_PWM);
  uint g_slice_num = pwm_gpio_to_slice_num(GREEN_LED_PIN);  // should be 6 A
  gpio_set_function(YELLOW_LED_PIN, GPIO_FUNC_PWM);
  uint y_slice_num = pwm_gpio_to_slice_num(YELLOW_LED_PIN); // should be 6 B
  gpio_set_function(RED_LED_PIN, GPIO_FUNC_PWM);
  uint r_slice_num = pwm_gpio_to_slice_num(RED_LED_PIN);    // should be 2 A
  pwm_init(g_slice_num, &config, true);
  pwm_init(y_slice_num, &config, true);
  pwm_init(r_slice_num, &config, true);
  printf("VPN Status pwm LED pin set up on GPIO %d\n", GREEN_LED_PIN);
  printf("SVC Status pwm LED pin set up on GPIO %d\n", YELLOW_LED_PIN);
  printf("TEMP Status pwm LED pin set up on GPIO %d\n", RED_LED_PIN);

  while (1) {
    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    // read analog in adc, map to 255 scale, set analog out pwm
    uint16_t result = adc_read();
    long pwm_value = map(result, 0, 4095, 0, 255);
//    printf("Raw value: %d \t voltage: %f V \t PWM: %d\n", result, result * conversion_factor, pwm_value);
    pwm_set_chan_level(a_slice_num, PWM_CHAN_B, pwm_value);

    // if status has changed, output it on LEDs
    if ( STATUS_CHANGED ) {
        STATUS_CHANGED = false;  // reset until next interrupt
        // output status update
        printf("Received status: %d\n", status);
        // update analog io to output
        printf("Analog input Raw value: %d \t voltage: %f V \t PWM: %d\n", result, result * conversion_factor, pwm_value);
        // act on received status (green)
        if (status & (1 << GBIT)) {
            set_green_pwm_duty_cycle(g_slice_num, PWM_CHAN_A, GZ);
        } else {
            set_green_pwm_duty_cycle(g_slice_num, PWM_CHAN_A, 0.0f);
        }
        // act on received status (yellow)
        if (status & (1 << YBIT)) {
            set_yellow_pwm_duty_cycle(y_slice_num, PWM_CHAN_B, YZ);
        } else {
            set_yellow_pwm_duty_cycle(y_slice_num, PWM_CHAN_B, 0.0f);
        }
        // act on received status (green)
        if (status & (1 << RBIT)) {
            set_red_pwm_duty_cycle(r_slice_num, PWM_CHAN_A, RZ);
        } else {
            set_red_pwm_duty_cycle(r_slice_num, PWM_CHAN_A, 0.0f);
        }
    } // end if status changed
    sleep_ms(200);  // while loop delay (keep tight but give uart time to rx/tx)
  } // end while
  return 0;   // this should never execute because of while(1) above
} // end main
