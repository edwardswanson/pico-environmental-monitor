/**
 * @file lcd_pcf8574.h
 * @brief HD44780 LCD driver via PCF8574 I2C expander
 *
 * DATASHEET
 * https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf
 */

#ifndef LCD_PCF8574_H
#define LCD_PCF8574_H

#include <stdint.h>
#include "hardware/i2c.h"
#include <stdbool.h>

// I2C configuration
#define LCD_I2C_PORT i2c0
#define LCD_ADDR 0x27 ///< PCF8574 I2C address (A0-A2 low)

// PCF8574 pin mapping to LCD
#define LCD_RS_BIT 0x01        // P0: Register Select
#define LCD_ENABLE_BIT 0x04    // P2: Enable pulse
#define LCD_BACKLIGHT_BIT 0x08 // P3: Backlight control

// Public API
void lcd_init(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *s);
void lcd_backlight(bool on);

#endif // LCD_PCF8574_H
