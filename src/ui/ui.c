#include <stdio.h>
#include <math.h>
#include "ui.h"
#include "led_strip.h"


// G-R-B:
// 0xGGRRBB
#define PURPLE 0x00050E
#define BLUE 0x000011
#define TEAL 0x110011
#define GREEN 0x110000
#define YELLOW 0x111100
#define ORANGE 0x051100
#define RED 0x001100

#define BLACK 0x000000

// Temerature Levels (in degrees Fahrenheit):
#define TEMP_VERY_COLD 16
#define TEMP_COLD 32
#define TEMP_COOL 48
#define TEMP_MILD 64
#define TEMP_HOT 80
#define TEMP_VERY_HOT 96

/**
 * @brief Updates the LED array to display new humidity
 * @param humidity The new humidity to set
 */
static void update_led_array(float humidity)
{
    // clamp humidity to [0, 100]
    if (humidity < 0.0f)
        humidity = 0.0f;
    if (humidity > 100.0f)
        humidity = 100.0f;

    // convert humidity to number of LEDs
    uint8_t num_leds_on = (uint8_t)ceil(((humidity / 100.0f) * (NUM_LEDS - 1)));

    uint8_t i = 0;

    // set appropriate leds on
    for (; i < num_leds_on; i++)
    {
        led_on(leds[i]);
    }

    // clear remaining leds
    for (; i < NUM_LEDS; i++)
    {
        led_off(leds[i]);
    }

    // set 6th led if humidity is 100%
    if (humidity >= 100.0f)
        led_on(leds[NUM_LEDS - 1]);
}

/**
 * @brief Updates the LCD to display a new temperature and humidity
 * @param humidity The new humidity to set
 * @param temp The new temperature to set
 * @param temp_unit The unit symbol for the temperature (e.g. 'C' or 'F')
 */
static void update_lcd(float humidity, float temp, char temp_unit)
{
    char line1[17];
    char line2[17];

    // 16-char lines (pad with spaces to overwrite old characters)
    snprintf(line1, sizeof(line1), "Temp: %4.1f %c   ", temp, temp_unit);
    snprintf(line2, sizeof(line2), "Hum : %4.1f %%   ", humidity);

    lcd_set_cursor(0, 0);
    lcd_print(line1);
    lcd_set_cursor(0, 1);
    lcd_print(line2);
}


// *************************LED STRIP***********************************
static uint8_t curr_led_pattern = 2;  // 1 = light all same color, 2 = progressive fill

/**
 * @brief Get WS2812 color from temperature value
 * @param temp Temperature in Fahrenheit
 * @return 32-bit GRB color value (0xGGRRBB format)
 * @private
 */
static uint32_t get_temp_color(float temp) {
    if (temp < TEMP_VERY_COLD)      return PURPLE;
    else if (temp < TEMP_COLD)      return BLUE;
    else if (temp < TEMP_COOL)      return TEAL;
    else if (temp < TEMP_MILD)      return GREEN;
    else if (temp < TEMP_HOT)       return YELLOW;
    else if (temp < TEMP_VERY_HOT)  return ORANGE;
    else                            return RED;
}

/**
 * @brief Get number of LEDs to light from temperature value
 * @param temp Temperature in Celsius
 * @return uint8_t number
 * @private
 */
static uint8_t get_num_leds_to_light(float temp) {
    if (temp < TEMP_VERY_COLD)      return 2;
    else if (temp < TEMP_COLD)      return 3;
    else if (temp < TEMP_COOL)      return 4;
    else if (temp < TEMP_MILD)      return 5;
    else if (temp < TEMP_HOT)       return 6;
    else if (temp < TEMP_VERY_HOT)  return 7;
    else                            return 8;
}

/**
 * @brief 
 * Pattern 1: All LEDs same color based on temperature
 * Pattern 2: Progressive fill based on temperature
 * @param temp Temperature in Fahrenheit
 * 
 * Progressive fill chart:
 * | Temperature | LEDs | Color  |
 * |-------------|------|--------|
 * | < 16°F      | 2    | Purple | very cold
 * | 17 to 32°F  | 3    | Blue   | cold 
 * | 33 to 48°F  | 4    | Teal   | cool
 * | 49 to 64°F  | 5    | Green  | mild
 * | 65 to 80°F  | 6    | Yellow | hot
 * | 81 to 96°F  | 7    | Orange | very hot
 * | ≥ 96°F      | 8    | Red    | extreme heat *WARNING*
 */
static void update_led_strip(float temp, char temp_unit) {
    
    if (temp_unit == 'C') {
        temp = (temp * 9.0f / 5.0f) + 32.0f;
    }

    uint32_t color = get_temp_color(temp);

    if (curr_led_pattern == 1) {
        // Pattern 1: All LEDs same color
        led_strip_array_fill(color);
    } else { 
        // Pattern 2: Progressive fill
        led_strip_array_clear();
        uint8_t num_leds_to_light = get_num_leds_to_light(temp);
        led_strip_array_fill_partial(num_leds_to_light, color);
    }

    led_strip_light();

}

/**
 * @brief initializes all UI componenets
 *
 * UI componenets include an LED array and a LCD
 */
void ui_init(void)
{
    lcd_init();
    led_init();
    led_strip_init();
}

/**
 * @brief Calls a startup display for all UI components
 *
 * LCD should display bootup text
 * LEDS should all turn on then off
 */
void ui_startup(void)
{
    lcd_set_cursor(0, 0);
    lcd_print("Env Monitor");
    lcd_set_cursor(0, 1);
    lcd_print("Starting...");

    // test led array
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        led_on(leds[i]);
        sleep_ms(250);
    }

    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        led_off(leds[i]);
    }
}

/**
 * @brief Updates the UI based on a new humidity and temperature
 *
 * @param humidity The new humidity value
 * @param temp The new temperature value
 * @param temp_unit The unit symbol for the temperature (e.g. 'C' or 'F')
 */
void ui_update(float humidity, float temp, char temp_unit)
{
    update_lcd(humidity, temp, temp_unit);
    update_led_array(humidity);
    update_led_strip(temp, temp_unit);
}
