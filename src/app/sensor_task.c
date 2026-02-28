#include <stdbool.h>
#include <stdint.h>
#include "../drivers/dht20.h"
#include "pico/time.h"
#include "sensor_task.h"

static bool sensor_data_ready = false;
static struct repeating_timer sensor_timer;

// variables for sensor data return
static temp_unit_t current_temp_unit = TEMP_CELSIUS;
static bool mock_sensor = false;
static float mock_temp = 20.0;
static float mock_humid = 50.0;

/**
 * @brief callback function to read from the sensor
 *
 * @details This callback function sets the sensor_data_ready flag 
 * to inform the main loop that it should read temperature and humidity values.
 * A sensor read is not performed in this function due to i2c conflicts
 * j
 * @param t the repeating timer for the callback
 */
static bool sensor_task_callback(struct repeating_timer* t)
{
    sensor_data_ready = true;
    return true;
}

/**
*@brief initiate a repeating timer for a sensor read task
*
*/
void init_sensor_task(void)
{
    add_repeating_timer_ms(1000, sensor_task_callback, NULL, &sensor_timer);
}

/**
* @brief set the sensor to read a specific temperature unit (F or C)
*
* @param unit the unit to set the sensor temperature read to
*/
void set_temp_unit(uint8_t unit)
{
    current_temp_unit = (temp_unit_t)unit;
}

/**
* @brief convert a temperature from celcius to farenheit if needed
*
* @param celsius temperature in celsius to convert
*/
static float convert_temp(float celsius)
{
    if (current_temp_unit == TEMP_FAHRENHEIT)
    {
        return (celsius * 9.0f / 5.0f) + 32.0f;
    }
    return celsius;
}

/**
* @brief convert a temperature from celcius to farenheit if needed
*
* @param celsius temperature in celsius to convert
*/
static char get_unit_symbol(void)
{
    return (current_temp_unit == TEMP_FAHRENHEIT) ? 'F' : 'C';
}

/**
* @brief set a mock temperature for the ui to read
*
* @param temp temperature to set
*/
void set_mock_temp(float temp)
{
    mock_temp = temp;
    mock_sensor = true;

}

/**
* @brief set a mock humidity for the ui to read
*
* @param humid humidity to set
*/
void set_mock_humid(float humid)
{
    mock_humid = humid;
    mock_sensor = true;
}

/**
* @brief set the app into a mock mode to override DHT20 sensor read
*
* @param mock_status true to use mock values, false to use sensor values
*/
void set_mock_sensor(bool mock_status)
{
    mock_sensor = mock_status;
}

/**
* @brief Gets sensor data and stores a temperature, humidity and temp unit
*
* function only checks sensor data if the sensor_data_ready flag is set
* function checks if mock mode is enabled and collects values accordingly
*
* @param temp location to store the temperature value
* @param humidity location to store the humidity value
* @param temp_unit location to store the temperature unit
*
* @return true if sensor data was read, false if sensor data was not read
*/
bool read_sensor_data(float* temp, float* humidity, char* temp_unit)
{
    if (sensor_data_ready)
    {
        // check for mock mode otherwise read real sensor data
        if (mock_sensor)
        {
            *temp = mock_temp;
            *humidity = mock_humid;
        }
        else
        {
            float temp_celsius;
            dht20_read(humidity, &temp_celsius);
            // Convert temperature based on user preference
            *temp = convert_temp(temp_celsius);
        }
        *temp_unit = get_unit_symbol();
        sensor_data_ready = false;

        return true;
    }

    return false;
}
