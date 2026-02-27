#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "drivers/dht20.h"
#include "interfaces/command_interface.h"
#include "drivers/lcd_pcf8574.h"
#include "pico/time.h"
#include "ui/ui.h"

#define SDA_PIN 4
#define SCL_PIN 5

static bool sensor_data_ready = false;
static struct repeating_timer sensor_timer;
volatile absolute_time_t prev_time;


/**
 * @brief callback function to read from the sensor
 *
 * @details This callback function sets the sensor_data_ready flag 
 * to inform the main loop that it should read temperature and humidity values.
 * A sensor read is not performed in this function due to i2c conflicts
 * 
 * @param t the repeating timer for the callback
 */
static bool sensor_task_callback(struct repeating_timer* t)
{
    sensor_data_ready = true;
    prev_time = get_absolute_time();        // reset last time function was called
    return true;
}

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

    // app setup
    dht20_init();
    lcd_interface_init();
    ui_init();
    ui_startup();

    // task initilization
    add_repeating_timer_ms(1000, sensor_task_callback, NULL, &sensor_timer);

    sleep_ms(1200);

    while (true)
    {
        // Check for serial commands (non-blocking)
        lcd_interface_update();

        // error check sensor irq
        int64_t diff_us = absolute_time_diff_us(prev_time, get_absolute_time());
        if (diff_us > 1500000)
        {
            printf("Timer stalled\n");
        }

        if(sensor_data_ready)
        {
            float humidity, temp_celsius;
            dht20_read(&humidity, &temp_celsius);

            // Convert temperature based on user preference
            float temp_display = lcd_interface_convert_temp(temp_celsius);
            const char *unit = lcd_interface_get_unit_symbol();

            ui_update(humidity, temp_display, unit[0]); // Third param is to pass unit symbol (e.g. 'C' or 'F')

            sensor_data_ready = false;
        }

        sleep_ms(1);
    }
}
