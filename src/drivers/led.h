#include "pico/stdlib.h"

extern const uint8_t leds[];

#define NUM_LEDS 6
#define LED_ON 1
#define LED_OFF 0

void led_init();
void led_off(uint8_t led_pin);
void led_on(uint8_t led_pin);
