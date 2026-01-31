#include <stdio.h>
#include "ui.h"

void ui_init(void)
{
    lcd_init();
    led_init();
}

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
        sleep_ms(500);
    }

    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        led_off(leds[i]);
    }
}

void ui_update(float humidity, float temp)
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

    for (uint8_t i = 0; i < (uint8_t)humidity; i += 20)
    {
        led_on(leds[i]);
    }
}
