/**
 * @file led.c
 * @brief led driver for 6 stage led_array
 *
 */

#include "led.h"

const uint8_t leds[] = {10, 11, 12, 13, 14, 15};

/**
 * @brief Initialize the LED array 
 */
void led_init()
{
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        gpio_init(leds[i]);
        gpio_set_dir(leds[i], GPIO_OUT);
    }
}

/**
 * @brief Turn a LED on
 * @param led_pin The pin number corresponding to the LED
 */
void led_on(uint8_t led_pin)
{
    gpio_put(led_pin, LED_ON);
}

/**
 * @brief Turn a LED off
 * @param led_pin The pin number corresponding to the LED
 */
void led_off(uint8_t led_pin)
{
    gpio_put(led_pin, LED_OFF);
}
