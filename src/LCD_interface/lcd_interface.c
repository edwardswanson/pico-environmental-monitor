/*
 * lcd_interface.c
 *
 * LCD interface for the Pico Environmental Monitor.
 * This module handles the display of temperature and humidity on the LCD.
 *
 * How to connect (macOS):
 *   1) Connect the Pico via USB.
 *   2) Open a terminal and run: (requires brew install picocom)
 *        picocom /dev/tty.usbmodem* -b 115200
 *   3) Type commands and press Enter:
 *        F or FAHRENHEIT  -> Fahrenheit
 *        C or CELSIUS     -> Celsius
 *        H or HELP        -> list commands
 *   4) Exit screen with:
 *        Ctrl-a -> Ctrl-x
 *
 * Notes:
 * - Need to brew install picocom
 * - Commands are case-insensitive. You can use upper or lower case.
 * - Commands are processed on newline with a slight delay, so type slowly.
 */

#include "lcd_interface.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"

// Private state
static temp_unit_t current_temp_unit = TEMP_CELSIUS;
static char cmd_buffer[32];
static int cmd_index = 0;

// Initialize the LCD interface
void lcd_interface_init(void)
{
    current_temp_unit = TEMP_CELSIUS;
    cmd_index = 0;

    printf("\n=== Pico Environmental Monitor ===\n");
    printf("Available commands:\n");
    printf("  F / FAHRENHEIT  -> Fahrenheit\n");
    printf("  C / CELSIUS     -> Celsius\n");
    printf("  H / HELP        -> list commands\n");
    printf("  Q               -> show exit instructions\n");
    printf("\nEnter a command and press Enter:\n\n");
    fflush(stdout);
}

// Process a complete command
static void process_command(const char *cmd)
{
    if (strcasecmp(cmd, "F") == 0 || strcasecmp(cmd, "FAHRENHEIT") == 0)
    {
        current_temp_unit = TEMP_FAHRENHEIT;
        printf("Temperature unit set to Fahrenheit\n");
        fflush(stdout);
    }
    else if (strcasecmp(cmd, "C") == 0 || strcasecmp(cmd, "CELSIUS") == 0)
    {
        current_temp_unit = TEMP_CELSIUS;
        printf("Temperature unit set to Celsius\n");
        fflush(stdout);
    }
    else if (strcasecmp(cmd, "H") == 0 || strcasecmp(cmd, "HELP") == 0)
    {
        printf("\nAvailable commands:\n");
        printf("  F or FAHRENHEIT - Set temperature to Fahrenheit\n");
        printf("  C or CELSIUS    - Set temperature to Celsius\n");
        printf("  H or HELP       - Show this help message\n");
        printf("  Q               - Show exit instructions\n\n");
        fflush(stdout);
    }
    else if (strcasecmp(cmd, "Q") == 0)
    {
        printf("\nTo exit the terminal:\n");
        printf("  Press Ctrl-a then Ctrl-x\n");
        fflush(stdout);
    }
    else if (strlen(cmd) > 0)
    {
        printf("Unknown command: '%s' (type HELP for available commands)\n", cmd);
        fflush(stdout);
    }
}

// Check for and process serial input (non-blocking)
void lcd_interface_update(void)
{
    int c = getchar_timeout_us(0);

    if (c != PICO_ERROR_TIMEOUT)
    {
        if (c == '\n' || c == '\r')
        {
            if (cmd_index > 0)
            {
                cmd_buffer[cmd_index] = '\0';
                process_command(cmd_buffer);
                cmd_index = 0;
            }
        }
        else if (cmd_index < sizeof(cmd_buffer) - 1)
        {
            cmd_buffer[cmd_index++] = toupper(c);
        }
    }
}

// Get current temperature unit
temp_unit_t lcd_interface_get_temp_unit(void)
{
    return current_temp_unit;
}

// Convert temperature based on current unit
float lcd_interface_convert_temp(float celsius)
{
    if (current_temp_unit == TEMP_FAHRENHEIT)
    {
        return (celsius * 9.0f / 5.0f) + 32.0f;
    }
    return celsius;
}

// Get unit symbol string
const char *lcd_interface_get_unit_symbol(void)
{
    return (current_temp_unit == TEMP_FAHRENHEIT) ? "F" : "C";
}