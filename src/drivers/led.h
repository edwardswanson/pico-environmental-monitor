/**
 * @file led.h
 * @brief led driver for 6 stage led_array
 *
 */

#include "pico/stdlib.h"

extern const uint8_t leds[];

#define NUM_LEDS 6
#define LED_ON 1
#define LED_OFF 0

void led_init();
void turnoff_led(uint8_t led_pin);
void turnon_led(uint8_t led_pin);
