#include <stdio.h>
#include "sensor_task.h"
#include "../drivers/dht20.h"
#include "pico/time.h"
#include "pico/stdlib.h"  

static volatile float curr_humidity = 50.0;
static volatile float curr_temp = 25.0;
static volatile bool update_data = false;
static volatile bool mock = false;
static struct repeating_timer sensor_timer;

static bool sensor_task_callback(struct repeating_timer* t)
{
    update_data = true;
    return true;
}

void sensor_task_init()
{
    printf("starting timer init\n");
    // this starts the timer and initiates the callback
    add_repeating_timer_ms(1000, sensor_task_callback, NULL, &sensor_timer);
}

bool read_sensor_data(float* humidity, float* temp)
{
    if (update_data)
    {
        if (!mock) {
            dht20_read(humidity, temp);
        }
        else
        {
            *humidity = curr_humidity;
            *temp = curr_temp;
        }
        update_data = false;
        return true;
    }

    return false;
}

void set_humidity(float new_humidity)
{
    curr_humidity = new_humidity;
    mock = true;
}

void set_temp(float new_temp)
{
    curr_temp = new_temp;
    mock = true;
}

void mock_sensor(bool mock_on)
{
    mock = mock_on;
}
