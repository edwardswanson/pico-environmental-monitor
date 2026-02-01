#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include <stdbool.h>

// Command interface initialization
void cmd_init(void);

// Process any pending commands (call this in your main loop)
void cmd_process(void);

// Check if mock mode is enabled
bool cmd_is_mock_mode(void);

// Get mock values (only valid if mock mode is enabled)
void cmd_get_mock_values(float *humidity, float *temp);

// Command callback type for extensibility
typedef void (*cmd_callback_t)(const char *args);

// Register a new command (for future expansion)
void cmd_register(const char *name, cmd_callback_t callback);

#endif // COMMAND_INTERFACE_H
