/**
 * @file led_strip.c
 * @brief WS2812B RGB LED strip driver using Pico PIO
 *
 * Controls 8x WS2812B addressable LEDs (GP2) to visualize temperature
 *
 * Hardware:
 * - WS2812B Datasheet: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
 * - Adapted from Raspberry Pi Pico SDK WS2812 example
 * - GPIO 2 (data in), 5V power (external supply recommended for >8 LEDs)
 * EXTERNAL 5V POWER REQUIRED
 * USB cannot reliably power >3 WS2812 LEDs. Use external 5V supply for 8-LED strip.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "led_strip.h"


#define WS2812_PIN 2
#define NUM_STRIP_LEDS 8
#define IS_RGBW false


static uint32_t led_strip_array[NUM_STRIP_LEDS];

// Private global variabples for PIO state
static PIO pio;
static uint sm;
static uint offset;


/**
 * @brief Initialize WS2812B LED strip PIO state machine
 * 
 * Claims free PIO block and state machine, loads WS2812 program,
 * configures for 800kHz pixel clock on GPIO 2.
 * 
 * @note Must be called once before any led_pattern_*() functions
 * @warning Fails hard with hard_assert() if no PIO resources available
 */
void led_strip_init(void) {
        // This will find a free pio and state machine for our program and load it for us
        // We use pio_claim_free_sm_and_add_program_for_gpio_range (for_gpio_range variant)
        // so we will get a PIO instance suitable for addressing gpios >= 32 if needed and supported by the hardware
        bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
        if (!success) {
                printf("ERROR: Failed to clain PIO for WS2812\n");
                return;
        }
        ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
        led_strip_array_clear();
}

/**
 * @brief Sets ALL 'LEDs' in our led_strip_array with the desired color
 * @param color 32-bit GRB color value (0xGGRRBB format)
 */
void led_strip_array_fill(uint32_t color) {
        for (uint8_t i = 0; i < NUM_STRIP_LEDS; i++) {
                led_strip_array[i] = color;
        }
}

/**
 * @brief Sets partial 'LEDs' in our led_strip_array with the desired color
 * @param color 32-bit GRB color value (0xGGRRBB format)
 */
void led_strip_array_fill_partial(uint8_t num_leds_to_fill, uint32_t color) {
        if (num_leds_to_fill > NUM_STRIP_LEDS) {
                num_leds_to_fill = NUM_STRIP_LEDS;
        }
        for (uint8_t i = 0; i < num_leds_to_fill; i++) {
                led_strip_array[i] = color;
        }
}

/**
 * @brief Clear led_strip_array (all LEDs off)
 * Sets entire array to black (0x000000). 
 */
void led_strip_array_clear(void) {
        led_strip_array_fill(0); // sets all leds in the array to black (i.e. black == OFF)
}

/**
 * @brief Transmit the colors in led_strip_array to WS2812 hardware via PIO
 * Each 32-bit color value is shifted left 8 bits (<< 8u).
 */
void led_strip_light(void) {
        // Stream buffer to PIO
        for (uint8_t i = 0; i < NUM_STRIP_LEDS; i++) {
                pio_sm_put_blocking(pio, sm, led_strip_array[i] << 8u);
        }
}