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
    printf("Type HELP for available commands\n\n");
}

// Process a complete command
static void process_command(const char *cmd)
{
    if (strcasecmp(cmd, "F") == 0 || strcasecmp(cmd, "FAHRENHEIT") == 0)
    {
        current_temp_unit = TEMP_FAHRENHEIT;
        printf("Temperature unit set to Fahrenheit\n");
    }
    else if (strcasecmp(cmd, "C") == 0 || strcasecmp(cmd, "CELSIUS") == 0)
    {
        current_temp_unit = TEMP_CELSIUS;
        printf("Temperature unit set to Celsius\n");
    }
    else if (strcasecmp(cmd, "HELP") == 0)
    {
        printf("\nAvailable commands:\n");
        printf("  F or FAHRENHEIT - Set temperature to Fahrenheit\n");
        printf("  C or CELSIUS    - Set temperature to Celsius\n");
        printf("  HELP            - Show this help message\n\n");
    }
    else if (strlen(cmd) > 0)
    {
        printf("Unknown command: '%s' (type HELP for available commands)\n", cmd);
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