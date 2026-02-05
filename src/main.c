#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd_pcf8574.h"
#include "dht20.h"
#include "command_interface.h"

#define SDA_PIN 4
#define SCL_PIN 5

int main()
{
    stdio_init_all();

    // I2C setup
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    lcd_init();
    dht20_init();
    command_interface_init();

    lcd_set_cursor(0, 0);
    lcd_print("Env Monitor");
    lcd_set_cursor(0, 1);
    lcd_print("Starting...");
    sleep_ms(1200);

    while (true)
    {
        // Check for serial commands (non-blocking)
        command_interface_update();

        float humidity, temp_celsius;
        dht20_read(&humidity, &temp_celsius);

        // Convert temperature based on user preference
        float temp_display = command_interface_convert_temp(temp_celsius);
        const char *unit = command_interface_get_unit_symbol();

        char line1[17];
        char line2[17];

        // 16-char lines (pad with spaces to overwrite old characters)
        snprintf(line1, sizeof(line1), "Temp: %4.1f %s   ", temp_display, unit);
        snprintf(line2, sizeof(line2), "Hum : %4.1f %%   ", humidity);

        lcd_set_cursor(0, 0);
        lcd_print(line1);
        lcd_set_cursor(0, 1);
        lcd_print(line2);

        sleep_ms(1000);
    }
}