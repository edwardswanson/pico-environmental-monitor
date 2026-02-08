#include <stdio.h>
#include "app/sensor_task.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "drivers/dht20.h"
#include "ui/ui.h"
#include "tools/command/command_interface.h"
#include "tools/command/commands.c"
#include "app/sensor_task.h"

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

    dht20_init();

    ui_init();
    cmd_init();
    ui_startup(); 

    sensor_task_init();
    commands_init();

    sleep_ms(1200);

    while (true)
    {
        float humidity, temp;
        
        cmd_process();

        if (read_sensor_data(&humidity, &temp))
        {
            ui_update(humidity, temp);
        }

        sleep_ms(1);
    }
}
