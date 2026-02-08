/**
 * @file led_strip.h
 * @brief Public API for WS2812B LED strip driver
 * 
 * WS2812B Datasheet: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf [web:539]
 * 
 * Initialize once, then call patterns with temperature values from DHT20.
 */

#pragma once


void led_strip_init(void);
void led_pattern_1(float temp);
void led_pattern_2(float temp);