#ifndef LCD_INTERFACE_H
#define LCD_INTERFACE_H

// Temperature unit enum
typedef enum
{
    TEMP_CELSIUS,
    TEMP_FAHRENHEIT
} temp_unit_t;

// Initialize LCD interface
void lcd_interface_init(void);

// Check for and process incoming serial commands (call from main loop)
void lcd_interface_update(void);

// Get current temperature unit setting
temp_unit_t lcd_interface_get_temp_unit(void);

// Convert temperature based on current unit
float lcd_interface_convert_temp(float celsius);

// Get unit symbol string
const char *lcd_interface_get_unit_symbol(void);

#endif // LCD_INTERFACE_H