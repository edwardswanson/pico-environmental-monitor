#include "command_interface.h"
#include "../../app/sensor_task.h"
#include <stdio.h>

/*========================= COMMAND HANDLERS ============================ */

static void mock_temp(const int32_t args[])
{
    float temp = (float)args[0];
    
    set_temp(temp);
    mock_sensor(true);
    
    printf("OK: Mock temperature set to %.1f°C\n", temp);
}

static void mock_humid(const int32_t args[])
{
    float humidity = (float)args[0];

    set_humidity(humidity);
    mock_sensor(true);
    
    printf("OK: Mock humidity set to %.0f%%\n", humidity);
}

static void set_mock(const int32_t args[])
{
    mock_sensor(args[0]);
    
    printf("OK: Mock mode disabled (using real sensor)\n");
}


// Command definitions
static const cmd_entry_t sensor_commands[] = {
    { .name = "mock_temp", .handler = mock_temp, .num_args = 1, },
    { .name = "mock_humid", .handler = mock_humid, .num_args = 1, },
    { .name = "mock", .handler = set_mock, .num_args = 2, },
};

void commands_init(void)
{
    // Register each command
    for (size_t i = 0; i < sizeof(sensor_commands) / sizeof(sensor_commands[0]); i++) {
        cmd_register(&sensor_commands[i]);
    }
}

