#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "drivers/dht20.h"
#include "interfaces/command_interface.h"
#include "interfaces/commands.h"
#include "drivers/lcd_pcf8574.h"
#include "app/ui.h"
#include "app/sensor_task.h"

#define SDA_PIN 4
#define SCL_PIN 5
#define SENSOR_TIMEOUT_US 1500000

static bool sensor_data_ready = false;
static struct repeating_timer sensor_timer;
volatile absolute_time_t prev_time;

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
    cmd_init();
    commands_init();
    init_sensor_task();
    ui_init();
    ui_startup();

    sleep_ms(1200);

    while (true)
    {
        // Check for serial commands (non-blocking)
        cmd_process();

        // error check sensor irq
        int64_t diff_us = absolute_time_diff_us(prev_time, get_absolute_time());
        if (diff_us > SENSOR_TIMEOUT_US)
        {
            printf("Timer stalled\n");
        }

        float humidity, temp;
        char unit;

        if(read_sensor_data(&temp, &humidity, &unit))
        {
            ui_update(humidity, temp, unit); // Third param is to pass unit symbol (e.g. 'C' or 'F')
            prev_time = get_absolute_time();
        }

        sleep_ms(1);
    }
}
