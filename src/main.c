#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "drivers/dht20.h"
#include "LCD_interface/lcd_interface.h"
#include "drivers/lcd_pcf8574.h"
#include "ui/ui.h"

#define SDA_PIN 4
#define SCL_PIN 5

int main()
{
    stdio_init_all();
    sleep_ms(2000); // Wait for stdio to initialize (especially important for USB serial)

    // I2C setup
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    lcd_init();
    dht20_init();
    lcd_interface_init();

    ui_init();
    ui_startup();

    sleep_ms(1200);

    while (true)
    {
        // Check for serial commands (non-blocking)
        lcd_interface_update();

        float humidity, temp_celsius;
        dht20_read(&humidity, &temp_celsius);

        // Convert temperature based on user preference
        float temp_display = lcd_interface_convert_temp(temp_celsius);
        const char *unit = lcd_interface_get_unit_symbol();

        ui_update(humidity, temp_display, unit[0]); // Pass unit symbol for display

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
