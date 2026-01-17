#include "lcd_pcf8574.h"
#include "pico/stdlib.h"
#include <string.h>

static uint8_t g_backlight = LCD_BACKLIGHT_BIT;

static void i2c_write_byte(uint8_t data)
{
    i2c_write_blocking(LCD_I2C_PORT, LCD_ADDR, &data, 1, false);
}

static void pulse_enable(uint8_t data)
{
    i2c_write_byte(data | LCD_ENABLE_BIT | g_backlight);
    sleep_us(1);
    i2c_write_byte((data & ~LCD_ENABLE_BIT) | g_backlight);
    sleep_us(50);
}

// Send 4-bit nibble (upper 4 bits of LCD are carried on P4..P7)
static void write4bits(uint8_t nibble_with_ctrl)
{
    i2c_write_byte(nibble_with_ctrl | g_backlight);
    pulse_enable(nibble_with_ctrl);
}

static void send(uint8_t value, uint8_t mode_rs)
{
    uint8_t high = (value & 0xF0);
    uint8_t low = (uint8_t)((value << 4) & 0xF0);

    // Map D4..D7 to P4..P7 => just keep bits in upper nibble
    write4bits(high | mode_rs);
    write4bits(low | mode_rs);
}

static void command(uint8_t value)
{
    send(value, 0); // RS=0
}

static void write_char(char c)
{
    send((uint8_t)c, LCD_RS_BIT); // RS=1
}

void lcd_clear(void)
{
    command(0x01);
    sleep_ms(2);
}

void lcd_home(void)
{
    command(0x02);
    sleep_ms(2);
}

void lcd_set_cursor(uint8_t col, uint8_t row)
{
    // Row offsets for 16x2
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    command(0x80 | (col + row_offsets[row]));
}

void lcd_print(const char *s)
{
    for (size_t i = 0; i < strlen(s); i++)
    {
        if (s[i] == '\n')
        {
            // Move to second line at col 0 (common expectation)
            lcd_set_cursor(0, 1);
        }
        else
        {
            write_char(s[i]);
        }
    }
}

void lcd_init(void)
{
    // Give LCD time to power up
    sleep_ms(50);

    // Force into 4-bit mode sequence (per HD44780 init)
    // These are special because we're only sending upper nibbles initially.
    write4bits(0x30);
    sleep_ms(5);
    write4bits(0x30);
    sleep_us(150);
    write4bits(0x30);
    sleep_us(150);

    // Set 4-bit mode
    write4bits(0x20);
    sleep_us(150);

    // Function set: 4-bit, 2 line, 5x8 dots
    command(0x28);
    // Display control: display ON, cursor OFF, blink OFF
    command(0x0C);
    // Entry mode set: increment, no shift
    command(0x06);

    lcd_clear();
}