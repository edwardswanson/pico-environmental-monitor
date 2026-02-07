#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include <stdbool.h>

// Temperature unit enum
typedef enum {
    TEMP_CELSIUS,
    TEMP_FAHRENHEIT
} temp_unit_t;

// Initialize command interface
void command_interface_init(void);

// Check for and process incoming serial commands (call from main loop)
void command_interface_update(void);

// Get current temperature unit setting
temp_unit_t command_interface_get_temp_unit(void);

// Convert temperature based on current unit
float command_interface_convert_temp(float celsius);

// Get unit symbol string
const char* command_interface_get_unit_symbol(void);

#endif // COMMAND_INTERFACE_H