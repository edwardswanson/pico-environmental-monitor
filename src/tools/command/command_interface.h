#pragma once

#include <stdbool.h>
#include <stdint.h>

// Maximum number of arguments for a command
#define CMD_BUFFER_SIZE 128
#define MAX_COMMANDS 32
#define CMD_MAX_ARGS 16
#define CMD_MAX_LEN 16

// Command handler function type
typedef void (*cmd_handler_t)(const int32_t* args);

// Command table entry
typedef struct {
    const char* name;           // Command name (e.g., "mock_temp")
    cmd_handler_t handler;      // Function to call
    uint8_t num_args;      // Number of arguments expected
} cmd_entry_t;

void cmd_init(void);
void cmd_process(void);
void cmd_register(const cmd_entry_t* command);
