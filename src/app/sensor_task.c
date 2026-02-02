#include "sensor_task.h"
#include "../drivers/dht20.h"

static float curr_humidity = 50.0;
static float curr_temp = 25.0;
static bool data_ready = false;
static bool mock = false;
static struct repeating_timer sensor_timer;

bool sensor_task_callback()
{
    // Get sensor readings or use mock values
    if (!mock) {
        dht20_read(&curr_humidity, &curr_temp);
        data_ready = true;
    }

    return true;

}

void sensor_task_init()
{
    // this starts the timer and initiates the callback
    add_repeating_timer_ms(-1000, sensor_task_callback, NULL, &sensor_timer);
}

bool sensor_data_ready(void)
{
    return data_ready;
}

void read_sensor_data(float* humidity, float* temp)
{
    *humidity = curr_humidity;
    *temp = curr_temp;
    
    data_ready = false;
}

void set_humidity(float new_humidity)
{
    curr_humidity = new_humidity;
    data_ready = true;
    mock = true;
}

void set_temp(float new_temp)
{
    curr_temp = new_temp;
    data_ready = true;
    mock = true;
}

void mock_sensor(bool mock_on)
{
    mock = (mock_on) ? true : false;
}
