#pragma once

#include "../tools/command/command_interface.h"

void sensor_task_init();
bool sensor_data_ready();
bool read_sensor_data(float* humidity, float* temp);
void set_humidity(float new_humidity);
void set_temp(float new_temp);
void mock_sensor(bool mock_on);



