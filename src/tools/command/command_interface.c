#include "command_interface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"

// Command buffer for serial input
static char cmd_buffer[CMD_BUFFER_SIZE];
static int cmd_buffer_pos = 0;

// Command table
static const cmd_entry_t* command_table[MAX_COMMANDS];
static uint8_t command_count = 0;

// Forward declarations
static void cmd_execute(char* cmd_line);
static void help_handler(const int32_t* args);

// Built-in help command
static const cmd_entry_t help_command = {
    .name = "help",
    .handler = help_handler,
    .num_args = 0,
};

void cmd_init(void)
{
    cmd_register(&help_command);
    
    printf("\n=== Command Interface Ready ===\n");
    printf("Type 'help' for available commands\n\n");
}

void cmd_register(const cmd_entry_t* command)
{
    if (command_count >= MAX_COMMANDS) {
        printf("\nERROR: Command table full\n");
        return;
    }
    
    command_table[command_count++] = command;
}

/**
 * Parse arguments from command string
 * 
 * Parses space-separated arguments and converts to integers.
 * Supports:
 * - Decimal: "100"
 * - Hexadecimal: "0x64" or "0X64"
 * - Negative: "-5"
 * 
 * @param input String containing arguments
 * @param args Array to store parsed arguments
 * @param max_args Maximum number of arguments to parse
 * @return Number of arguments successfully parsed
 */
static uint8_t parse_args(char* input, int32_t args[], uint8_t max_args)
{
    uint8_t count = 0;
    char* token;
    char* saveptr;
    
    token = strtok_r(input, " ", &saveptr);
    
    while (token != NULL && count < max_args) {
        // Try to parse as integer
        char* endptr;
        long value;
        
        // Check for hex prefix
        if (strlen(token) > 2 && token[0] == '0' && 
            (token[1] == 'x' || token[1] == 'X')) {
            value = strtol(token, &endptr, 16);
        } else {
            value = strtol(token, &endptr, 10);
        }
        
        // Check if parsing was successful
        if (endptr != token && *endptr == '\0') {
            args[count++] = (int32_t)value;
        } else {
            printf("WARNING: Could not parse '%s' as integer\n", token);
        }
        
        token = strtok_r(NULL, " ", &saveptr);
    }
    
    return count;
}

/**
 * Find and execute a command
 */
static void cmd_execute(char* cmd_line)
{
    // Skip leading whitespace
    while (*cmd_line == ' ' || *cmd_line == '\t') {
        cmd_line++;
    }
    
    // Empty command
    if (*cmd_line == '\0') {
        return;
    }
    
    // Find space or end of command name
    char* space = strchr(cmd_line, ' ');
    char* args_str = NULL;
    
    if (space != NULL) {
        *space = '\0';
        args_str = space + 1;
        
        // Skip whitespace before args
        while (*args_str == ' ' || *args_str == '\t') {
            args_str++;
        }
    }
    
    // Convert command to lowercase for case-insensitive matching
    for (char* p = cmd_line; *p; p++) {
        if (*p >= 'A' && *p <= 'Z') {
            *p = *p - 'A' + 'a';
        }
    }
    
    // Search command table
    for (uint8_t i = 0; i < command_count; i++) {
        if (strcmp(cmd_line, command_table[i]->name) == 0) {
            // Found command - parse arguments
            int32_t args[CMD_MAX_ARGS] = {0};
            uint8_t num_args = 0;
            
            if (args_str != NULL && *args_str != '\0') {
                num_args = parse_args(args_str, args, CMD_MAX_ARGS);
            }
            
            // Validate argument count
            if (num_args != command_table[i]->num_args) {
                printf("\nERROR: Command '%s' expects %d arguments, got %d\n",
                       command_table[i]->name,
                       command_table[i]->num_args,
                       num_args);
                return;
            }
            
            // Execute command
            command_table[i]->handler(args);
            return;
        }
    }
    
    // Unknown command
    printf("\nERROR: Unknown command '%s'. Type 'help' for available commands.\n", 
           cmd_line);
}

void cmd_process(void)
{
    uint8_t chars_processed = 0;
    char cmd_line[CMD_BUFFER_SIZE];
    uint8_t cmd_line_pos = 0;
    
    // read pipe up to max characters
    while (chars_processed < CMD_BUFFER_SIZE) 
    {
        int c = getchar_timeout_us(0);
        
        // No data
        if (c == PICO_ERROR_TIMEOUT) {
            return; 
        }
        
        chars_processed++;
        
        // process and execute command on newline/return
        if (c == '\n' || c == '\r') 
        {
            cmd_line[cmd_line_pos++] = '\0';
            cmd_execute(cmd_line);
            return;
        }

        cmd_line[cmd_line_pos++] = c;
    }
}

// Built-in command handlers

static void help_handler(const int32_t* args)
{
    printf("\nRegistered Commands (%d):\n", command_count);
    for (uint8_t i = 0; i < command_count; i++) {
        printf("  %d. %s (%d args)\n", 
               i + 1, 
               command_table[i]->name,
               command_table[i]->num_args);
    }
    printf("\n");
}
