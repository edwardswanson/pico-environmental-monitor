/**
 * @file led_strip.c
 * @brief WS2812B RGB LED strip driver using Pico PIO
 *
 * Controls 8x WS2812B addressable LEDs (GP2) to visualize temperature:
 * - Pattern 1: All LEDs same color (temp → color map)
 * - Pattern 2: Progressive fill (temp → #LEDs lit + color)
 *
 * Hardware:
 * - WS2812B Datasheet: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
 * - Adapted from Raspberry Pi Pico SDK WS2812 example
 * - GPIO 2 (data in), 5V power (external supply recommended for >8 LEDs)
 * 
 * API Usage:
 *   led_strip_init();     // Call once from ui_init()
 *   led_pattern_1(temp);  // All LEDs same color
 *   led_pattern_2(temp);  // Progressive fill pattern
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "led_strip.h"


#define WS2812_PIN 2
#define NUM_LEDS 8
#define IS_RGBW false

// G-R-B:
// 0xGGRRBB
#define PURPLE 0x00050E
#define BLUE 0x000011
#define TEAL 0x110011
#define GREEN 0x110000
#define YELLOW 0x111100
#define ORANGE 0x051100
#define RED 0x001100

#define BLACK 0x000000

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
        hard_assert(success);

        ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
}



/**
 * @brief Pattern 1: All LEDs same color based on temperature
 * @param temp Temperature in Celsius (-40 to +85°C range)
 * 
 * Maps temperature to single color across all 8 LEDs
*/
void led_pattern_1(float temp) {
        uint32_t color;
        color = temp < -10 ? PURPLE : 
                temp < -2 ? BLUE : 
                temp < 6 ? TEAL : 
                temp < 14 ? GREEN : 
                temp < 22 ? YELLOW : 
                temp < 29 ? ORANGE : RED;

        for (int i = 0; i < NUM_LEDS; i++) {
                pio_sm_put_blocking(pio, sm, color << 8);
        }
}


/**
 * @brief Get WS2812 color from temperature value
 * @param temp Temperature in Celsius
 * @return 32-bit GRB color value (0xGGRRBB format)
 * @private
 */
static uint32_t get_temp_color(float temp) {
    return temp < -10 ? PURPLE :
           temp < -2 ? BLUE  :
           temp < 6 ? TEAL  :
           temp < 14 ? GREEN :
           temp < 22 ? YELLOW :
           temp < 29 ? ORANGE : RED;
}

/**
 * @brief Number of LEDs to illuminate per temperature color
 * @private
 */
static const uint8_t color_led_count[] = {
    [PURPLE] = 2,
    [BLUE]   = 3,
    [TEAL]   = 4,
    [GREEN]  = 5,
    [YELLOW] = 6,
    [ORANGE] = 7,
    [RED]    = NUM_LEDS  // all LEDs
};

/**
 * @brief Fill first N LEDs with color, rest black
 * @param color 32-bit GRB color value
 * @param num_leds Number of LEDs to illuminate [0, NUM_LEDS]
 * @private
 */
static void fill_leds(uint32_t color, uint8_t num_leds) {
    // Light first N LEDs
    for (uint8_t i = 0; i < num_leds; i++) {
        pio_sm_put_blocking(pio, sm, color << 8u);
    }
    
    // Clear remaining LEDs
    for (uint8_t i = num_leds; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(pio, sm, BLACK << 8u);
    }
}

/**
 * @brief Pattern 2: Progressive fill based on temperature
 * @param temp Temperature in Celsius
 * 
 * Lights progressive number of LEDs (2-8) with temperature color:
 * | Temperature | LEDs | Color  |
 * |-------------|------|--------|
 * | < -10°C     | 2    | Purple |
 * | -10 to -2°C | 3    | Blue   |
 * | -2 to 6°C   | 4    | Teal   |
 * | 6 to 14°C   | 5    | Green  |
 * | 14 to 22°C  | 6    | Yellow |
 * | 22 to 29°C  | 7    | Orange |
 * | ≥ 29°C      | 8    | Red    |
 */
void led_pattern_2(float temp) {
        uint32_t color = get_temp_color(temp);
        uint8_t num_leds = color_led_count[color];
        fill_leds(color, num_leds);
}