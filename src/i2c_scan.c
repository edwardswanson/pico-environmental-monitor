#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5

int main()
{
    stdio_init_all();

    // Wait until a serial terminal connects over USB
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }
    sleep_ms(200); // small settle time after connect

    printf("I2C scan starting...\n");

    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("I2C scan starting...\n");

    while (true)
    {
        printf("Scanning...\n");
        for (int addr = 0x08; addr <= 0x77; addr++)
        {
            uint8_t rx;
            int ret = i2c_read_blocking(I2C_PORT, addr, &rx, 1, false);
            if (ret >= 0)
            {
                printf("Found device at 0x%02X\n", addr);
            }
        }
        sleep_ms(3000);
    }
}
