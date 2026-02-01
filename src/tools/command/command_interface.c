#include "command_interface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#define CMD_BUFFER_SIZE 128
#define MAX_COMMANDS 16

// Mock mode state
static bool mock_mode_enabled = false;
static float mock_humidity = 50.0f;
static float mock_temp = 25.0f;

// Command buffer for serial input
static char cmd_buffer[CMD_BUFFER_SIZE];
static int cmd_buffer_pos = 0;

// Command table structure
typedef struct {
    char name[16];
    cmd_callback_t callback;
} command_entry_t;

static command_entry_t command_table[MAX_COMMANDS];
static int command_count = 0;

// Forward declarations of built-in commands
static void cmd_mock_temp(const char *args);
static void cmd_mock_humid(const char *args);
static void cmd_mock_off(const char *args);
static void cmd_help(const char *args);
static void cmd_status(const char *args);

void cmd_init(void)
{
    // stdio_init_all() should already be called in main
    
    // Register built-in commands
    cmd_register("MOCK_TEMP", cmd_mock_temp);
    cmd_register("MOCK_HUMID", cmd_mock_humid);
    cmd_register("MOCK_OFF", cmd_mock_off);
    cmd_register("HELP", cmd_help);
    cmd_register("STATUS", cmd_status);
    
    printf("\n=== DHT20 Command Interface Ready ===\n");
    printf("Type HELP for available commands\n\n");
}

void cmd_register(const char *name, cmd_callback_t callback)
{
    if (command_count >= MAX_COMMANDS) {
        printf("ERROR: Command table full\n");
        return;
    }
    
    strncpy(command_table[command_count].name, name, 15);
    command_table[command_count].name[15] = '\0';
    command_table[command_count].callback = callback;
    command_count++;
}

static void cmd_execute(char *cmd_line)
{
    // Skip leading whitespace
    while (*cmd_line == ' ' || *cmd_line == '\t') {
        cmd_line++;
    }
    
    // Empty command
    if (*cmd_line == '\0') {
        return;
    }
    
    // Find the end of the command name
    char *space = strchr(cmd_line, ' ');
    char *args = NULL;
    
    if (space != NULL) {
        *space = '\0';
        args = space + 1;
        // Skip whitespace in args
        while (*args == ' ' || *args == '\t') {
            args++;
        }
    }
    
    // Convert command to uppercase for case-insensitive matching
    for (char *p = cmd_line; *p; p++) {
        if (*p >= 'a' && *p <= 'z') {
            *p = *p - 'a' + 'A';
        }
    }
    
    // Search command table
    for (int i = 0; i < command_count; i++) {
        if (strcmp(cmd_line, command_table[i].name) == 0) {
            command_table[i].callback(args);
            return;
        }
    }
    
    printf("ERROR: Unknown command '%s'. Type HELP for available commands.\n", cmd_line);
}

void cmd_process(void)
{
    // Non-blocking check for serial input
    int c = getchar_timeout_us(0);
    
    if (c == PICO_ERROR_TIMEOUT) {
        return; // No character available
    }
    
    if (c == '\n' || c == '\r') {
        // End of command
        if (cmd_buffer_pos > 0) {
            cmd_buffer[cmd_buffer_pos] = '\0';
            cmd_execute(cmd_buffer);
            cmd_buffer_pos = 0;
        }
        return;
    }
    
    if (c == 8 || c == 127) { // Backspace or DEL
        if (cmd_buffer_pos > 0) {
            cmd_buffer_pos--;
            printf("\b \b"); // Erase character on terminal
        }
        return;
    }
    
    // Add character to buffer
    if (cmd_buffer_pos < CMD_BUFFER_SIZE - 1) {
        cmd_buffer[cmd_buffer_pos++] = (char)c;
        putchar(c); // Echo character
    }
}

bool cmd_is_mock_mode(void)
{
    return mock_mode_enabled;
}

void cmd_get_mock_values(float *humidity, float *temp)
{
    *humidity = mock_humidity;
    *temp = mock_temp;
}

// Built-in command implementations
static void cmd_mock_temp(const char *args)
{
    if (args == NULL || *args == '\0') {
        printf("ERROR: MOCK_TEMP requires a temperature value\n");
        printf("Usage: MOCK_TEMP <value>\n");
        return;
    }
    
    float value = atof(args);
    mock_temp = value;
    mock_mode_enabled = true;
    
    printf("OK: Mock temperature set to %.2f°C (Mock mode: ON)\n", mock_temp);
}

static void cmd_mock_humid(const char *args)
{
    if (args == NULL || *args == '\0') {
        printf("ERROR: MOCK_HUMID requires a humidity value\n");
        printf("Usage: MOCK_HUMID <value>\n");
        return;
    }
    
    float value = atof(args);
    
    if (value < 0.0f || value > 100.0f) {
        printf("WARNING: Humidity value %.2f is outside normal range (0-100%%)\n", value);
    }
    
    mock_humidity = value;
    mock_mode_enabled = true;
    
    printf("OK: Mock humidity set to %.2f%% (Mock mode: ON)\n", mock_humidity);
}

static void cmd_mock_off(const char *args)
{
    (void)args; // Unused
    
    mock_mode_enabled = false;
    printf("OK: Mock mode disabled (using real sensor)\n");
}

static void cmd_help(const char *args)
{
    (void)args; // Unused
    
    printf("\n=== Available Commands ===\n");
    printf("MOCK_TEMP <value>   - Set mock temperature (enables mock mode)\n");
    printf("MOCK_HUMID <value>  - Set mock humidity (enables mock mode)\n");
    printf("MOCK_OFF            - Disable mock mode (use real sensor)\n");
    printf("STATUS              - Show current mock mode status\n");
    printf("HELP                - Show this help message\n");
    printf("\nExamples:\n");
    printf("  MOCK_TEMP 28.5\n");
    printf("  MOCK_HUMID 75\n");
    printf("  MOCK_OFF\n");
    printf("\nCommands are case-insensitive.\n\n");
}

static void cmd_status(const char *args)
{
    (void)args; // Unused
    
    printf("\n=== System Status ===\n");
    printf("Mock mode: %s\n", mock_mode_enabled ? "ENABLED" : "DISABLED");
    
    if (mock_mode_enabled) {
        printf("Mock temperature: %.2f°C\n", mock_temp);
        printf("Mock humidity: %.2f%%\n", mock_humidity);
    } else {
        printf("Using real DHT20 sensor readings\n");
    }
    printf("\n");
}
