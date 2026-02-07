#include <stdio.h>
#include <math.h>
#include "ui.h"

/**
 * @brief Updates the LED array to display new humidity
 * @param humidity The new humidity to set
 */
static void update_led_array(float humidity)
{
    // clamp humidity to [0, 100]
    if (humidity < 0.0f) humidity = 0.0f;
    if (humidity > 100.0f) humidity = 100.0f;

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
    if (humidity >= 100.0f) led_on(leds[NUM_LEDS - 1]);
}

/**
 * @brief Updates the LCD to display a new temperature and humidity
 * @param humidity The new humidity to set
 * @param temp The new temperature to set
 */
static void update_lcd(float humidity, float temp)
{
    char line1[17];
    char line2[17];

    // 16-char lines (pad with spaces to overwrite old characters)
    snprintf(line1, sizeof(line1), "Temp: %4.1f C   ", temp);
    snprintf(line2, sizeof(line2), "Hum : %4.1f %%   ", humidity);

    lcd_set_cursor(0, 0);
    lcd_print(line1);
    lcd_set_cursor(0, 1);
    lcd_print(line2);
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
 * 
 */
void ui_update(float humidity, float temp)
{
    update_lcd(humidity, temp);
    update_led_array(humidity);
}
