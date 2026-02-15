#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "drivers/dht20.h"
#include "interfaces/command_interface.h"
#include "drivers/lcd_pcf8574.h"
#include "ui/ui.h"

#define SDA_PIN 4
#define SCL_PIN 5

int main()
{
    stdio_init_all();

    // Wait for USB stdio connection with a bounded timeout (max 1 second)
    // This avoids unnecessary 2s delays when USB isn't being used
    absolute_time_t deadline = make_timeout_time_ms(1000);
    while (!stdio_usb_connected() && absolute_time_diff_us(get_absolute_time(), deadline) > 0)
    {
        sleep_ms(10);
    }

    // I2C setup
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

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

        ui_update(humidity, temp_display, unit[0]); // Third param is to pass unit symbol (e.g. 'C' or 'F')

        sleep_ms(1000);
    }
}
