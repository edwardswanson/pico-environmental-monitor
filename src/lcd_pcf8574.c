/**
 * @file lcd_pcf8574.c
 * @brief Driver for HD44780-compatible LCD via PCF8574 I2C I/O expander
 *
 * This driver uses 4-bit mode to communicate with a 16x2 LCD display
 * through a PCF8574 I2C backpack.
 */

#include "lcd_pcf8574.h"
#include "pico/stdlib.h"
#include <string.h>

// Timing constants (per HD44780 datasheet)
#define LCD_POWER_ON_DELAY_MS 50 ///< Power-up stabilization time
#define LCD_CLEAR_DELAY_MS 2     ///< Clear/home command execution time
#define LCD_ENABLE_PULSE_US 1    ///< Enable pulse width (min 450ns)
#define LCD_SETTLE_DELAY_US 50   ///< Command settling time

// I2C retry configuration
#define I2C_MAX_RETRIES 3    ///< Max retry attempts for failed I2C writes
#define I2C_RETRY_DELAY_MS 1 ///< Delay between retries

// HD44780 command codes
#define LCD_CMD_CLEAR 0x01        ///< Clear display
#define LCD_CMD_HOME 0x02         ///< Return cursor home
#define LCD_CMD_ENTRY_MODE 0x06   ///< Entry mode: increment, no shift
#define LCD_CMD_DISPLAY_ON 0x0C   ///< Display on, cursor off, blink off
#define LCD_CMD_FUNCTION_SET 0x28 ///< 4-bit mode, 2 lines, 5x8 font
#define LCD_CMD_SET_DDRAM 0x80    ///< Set DDRAM address (OR with address)

// 4-bit mode initialization sequence
#define LCD_INIT_8BIT 0x30 ///< Force 8-bit mode
#define LCD_INIT_4BIT 0x20 ///< Switch to 4-bit mode

static uint8_t g_backlight = LCD_BACKLIGHT_BIT;

/**
 * @brief Write a single byte to the I2C bus with retry logic
 * @param data Byte to write to PCF8574
 * @return true if successful, false after max retries
 */
static bool i2c_write_byte(uint8_t data)
{
    for (int attempt = 0; attempt < I2C_MAX_RETRIES; attempt++)
    {
        int result = i2c_write_blocking(LCD_I2C_PORT, LCD_ADDR, &data, 1, false);

        if (result == 1) // Success: 1 byte written
        {
            return true;
        }

        printf("[I2C] Attempt %d failed (result=%d), retrying...\n", attempt + 1, result);
        // Retry on failure (NACK, bus error, timeout)
        if (attempt < I2C_MAX_RETRIES - 1)
        {
            sleep_ms(I2C_RETRY_DELAY_MS);
        }
    }

    // All retries failed (use printf until better approach available)
    printf("[I2C] Failed to write 0x%02X after %d attempts\n", data, I2C_MAX_RETRIES);
    return false;
}

/**
 * @brief Generate an enable pulse to latch data into LCD
 * @param data Data byte with control bits set
 */
static void pulse_enable(uint8_t data)
{
    i2c_write_byte(data | LCD_ENABLE_BIT | g_backlight);
    sleep_us(LCD_ENABLE_PULSE_US);
    i2c_write_byte((data & ~LCD_ENABLE_BIT) | g_backlight);
    sleep_us(LCD_SETTLE_DELAY_US);
}

/**
 * @brief Send a 4-bit nibble to the LCD
 * @param nibble_with_ctrl Upper nibble contains data, lower bits contain control signals
 */
static void write4bits(uint8_t nibble_with_ctrl)
{
    i2c_write_byte(nibble_with_ctrl | g_backlight);
    pulse_enable(nibble_with_ctrl);
}

/**
 * @brief Send full 8-bit value to LCD in two 4-bit operations
 * @param value 8-bit data/command
 * @param mode_rs Register select: 0=command, LCD_RS_BIT=data
 */
static void send(uint8_t value, uint8_t mode_rs)
{
    uint8_t high = (value & 0xF0);                // Upper nibble
    uint8_t low = (uint8_t)((value << 4) & 0xF0); // Lower nibble shifted

    write4bits(high | mode_rs);
    write4bits(low | mode_rs);
}

/**
 * @brief Send a command byte to the LCD (RS=0)
 * @param value Command byte
 */
static void command(uint8_t value)
{
    send(value, 0);
}

/**
 * @brief Write character to LCD at current cursor position (RS=1)
 * @param c Character to display
 */
static void write_char(char c)
{
    send((uint8_t)c, LCD_RS_BIT);
}

/**
 * @brief Clear LCD display and return cursor to home
 */
void lcd_clear(void)
{
    command(LCD_CMD_CLEAR);
    sleep_ms(LCD_CLEAR_DELAY_MS);
}

/**
 * @brief Return cursor to home position (0,0)
 */
void lcd_home(void)
{
    command(LCD_CMD_HOME);
    sleep_ms(LCD_CLEAR_DELAY_MS); // Home command needs extra time
}

/**
 * @brief Set cursor position
 * @param col Column (0-15 for 16x2 LCD)
 * @param row Row (0-1 for 16x2 LCD)
 */
void lcd_set_cursor(uint8_t col, uint8_t row)
{
    // DDRAM addresses for 16x2 LCD
    // Row 0: 0x00-0x0F, Row 1: 0x40-0x4F
    static const uint8_t row_offsets[] = {0x00, 0x40};

    if (row < 2 && col < 16) // Bounds check
    {
        command(LCD_CMD_SET_DDRAM | (col + row_offsets[row]));
    }
}

/**
 * @brief Print null-terminated string to LCD
 * @param s String to display (supports '\n' for second line)
 */
void lcd_print(const char *s)
{
    if (!s)
        return; // Null pointer guard

    for (size_t i = 0; s[i] != '\0'; i++)
    {
        if (s[i] == '\n')
        {
            lcd_set_cursor(0, 1); // Move to second line
        }
        else
        {
            write_char(s[i]);
        }
    }
}

/**
 * @brief Control LCD backlight
 * @param on true=backlight on, false=backlight off
 */
void lcd_backlight(bool on)
{
    g_backlight = on ? LCD_BACKLIGHT_BIT : 0;
    i2c_write_byte(g_backlight); // Update backlight immediately
}

/**
 * @brief Initialize LCD in 4-bit mode
 *
 * Follows HD44780 initialization sequence:
 * 1. Wait for power-up
 * 2. Force 8-bit mode (3x)
 * 3. Switch to 4-bit mode
 * 4. Configure display parameters
 * 5. Clear display
 */
void lcd_init(void)
{
    // Wait for LCD power-up (VCC rise time + internal reset)
    sleep_ms(LCD_POWER_ON_DELAY_MS);

    // Initialization sequence per HD44780 datasheet fig. 24
    // (Required to sync LCD into known 4-bit mode state)
    write4bits(LCD_INIT_8BIT);
    sleep_ms(5);
    write4bits(LCD_INIT_8BIT);
    sleep_us(150);
    write4bits(LCD_INIT_8BIT);
    sleep_us(150);

    // Now switch to 4-bit mode
    write4bits(LCD_INIT_4BIT);
    sleep_us(150);

    // Function set: 4-bit interface, 2 lines, 5x8 font
    command(LCD_CMD_FUNCTION_SET);

    // Display control: display on, cursor off, blink off
    command(LCD_CMD_DISPLAY_ON);

    // Entry mode: increment cursor, no display shift
    command(LCD_CMD_ENTRY_MODE);

    // Clear display
    lcd_clear();
}