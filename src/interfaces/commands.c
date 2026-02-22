#include "command_interface.h"
#include "../app/sensor_task.h"
#include <stdio.h>

static void mock_temp(const int32_t args[])
{
    float temp = (float)args[0];
    temp += ((float)args[1] / 10);
    
    set_mock_temp(temp);
    set_mock_sensor(true);
    
    printf("OK: Mock temperature set to %.1f°C\n", temp);
}

static void mock_humid(const int32_t args[])
{
    float humidity = (float)args[0];
    humidity += ((float)args[1] / 10);

    set_mock_humid(humidity);
    set_mock_sensor(true);
    
    printf("OK: Mock humidity set to %.0f%%\n", humidity);
}

static void mock_sens(const int32_t args[])
{
    set_mock_sensor(args[0]);
    
    printf("OK: Mock mode disabled (using real sensor)\n");
}

static void set_unit(const int32_t args[])
{
    set_temp_unit(args[0]);
    
    printf("OK: unit set");
}
// Command definitions
static const cmd_entry_t sensor_commands[] = {
    { .name = "temp", .handler = mock_temp, .num_args = 2, },
    { .name = "humid", .handler = mock_humid, .num_args = 2, },
    { .name = "mock", .handler = mock_sens, .num_args = 1, },
    { .name = "unit", .handler = set_unit, .num_args = 1, },
};

void commands_init(void)
{
    // Register each command
    for (size_t i = 0; i < sizeof(sensor_commands) / sizeof(sensor_commands[0]); i++) {
        cmd_register(&sensor_commands[i]);
    }
}
