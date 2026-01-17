#pragma once
#include <stdint.h>
#include "hardware/i2c.h"

// Change these if you used different pins/port
#define LCD_I2C_PORT i2c0

// Change this to your scan result (common: 0x27 or 0x3F)
#define LCD_ADDR 0x27

// Common PCF8574 -> LCD mapping:
// P0=RS, P1=RW, P2=E, P3=Backlight, P4=D4, P5=D5, P6=D6, P7=D7
#define LCD_BACKLIGHT_BIT 0x08
#define LCD_ENABLE_BIT 0x04
#define LCD_RW_BIT 0x02
#define LCD_RS_BIT 0x01

void lcd_init(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *s);