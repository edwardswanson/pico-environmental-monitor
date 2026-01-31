#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "drivers/dht20.h"
#include "ui/ui.h"

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
    ui_startup(); 

    sleep_ms(1200);

    while (true)
    {
        float humidity, temp;
        dht20_read(&humidity, &temp);

        ui_update(humidity, temp);
        sleep_ms(1000);
    }
}
