/**
 * @file led_strip.h
 * @brief Public API for WS2812B LED strip driver
 * 
 * WS2812B Datasheet: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
 * 
 * Initialize once, then call patterns with temperature values from DHT20.
 */

#pragma once


void led_strip_init(void);
void led_strip_array_fill(uint32_t color);
void led_strip_array_fill_partial(uint8_t num_leds_to_fill, uint32_t color);
void led_strip_array_clear(void);
void led_strip_light(void);