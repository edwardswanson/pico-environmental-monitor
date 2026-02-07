#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "command_interface.h"
#include "parse.h"

// Command buffer for serial input
static char cmd_buffer[CMD_BUFFER_SIZE];
static int cmd_buffer_pos = 0;

// Command table
static const cmd_entry_t* command_table[MAX_COMMANDS];
static uint8_t command_count = 0;

// Forward declarations
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
 * Find and execute a command
 */
static void cmd_execute(char* cmd, int32_t* args, uint8_t num_args)
{
    // search command table for a match
    for (uint8_t i = 0; i < command_count; i++) 
    {
        if (strcmp(cmd, command_table[i]->name) == 0) 
        {
            // Validate argument count
            if (num_args != command_table[i]->num_args) 
            {
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
           cmd);
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

            char cmd[CMD_MAX_LEN];
            int32_t args[CMD_MAX_ARGS];
            uint8_t num_args;

            num_args = parse_line(cmd_line, cmd, args);
            cmd_execute(cmd, args, num_args);
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
