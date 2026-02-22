#pragma once

// Temperature unit enum
typedef enum
{
    TEMP_CELSIUS,
    TEMP_FAHRENHEIT
} temp_unit_t;

bool read_sensor_data(float* temp, float* humidity, char* temp_unit);

void init_sensor_task(void);
void set_mock_temp(float temp);
void set_mock_humid(float humid);
void set_mock_sensor(bool mock_status);
void set_temp_unit(uint8_t unit);
