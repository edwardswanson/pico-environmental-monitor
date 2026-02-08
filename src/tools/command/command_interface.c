#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "command_interface.h"
#include "parse.h"

// Command table
static const cmd_entry_t* command_table[MAX_COMMANDS];
static uint8_t command_count = 0;

// Built-in help command
static void help_handler(const int32_t* args);
static const cmd_entry_t help_command = {
    .name = "help",
    .handler = help_handler,
    .num_args = 0,
};

/**
 * @brief init the command table with the help command
 */
void cmd_init(void)
{
    cmd_register(&help_command);
    
    printf("\n=== Command Interface Ready ===\n");
    printf("Type 'help' for available commands\n\n");
}

/**
 * @brief register a new command onto the command table
 *
 * @param command the command entry to add to the tabl 
 */
void cmd_register(const cmd_entry_t* command)
{
    if (command_count >= MAX_COMMANDS) {
        printf("\nERROR: Command table full\n");
        return;
    }
    
    command_table[command_count++] = command;
}

/**
 * @brief execute a command from the command interface
 *
 * function will check the number of arguments with the value
 * listed in the command table, and send an error message if
 * the numbers don't match
 *
 * @param cmd the command to execute
 * @param args pointer to the arguments for the command
 * @param num_args number of arguments for the command
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

/**
 * @brief process the command string from the usb port
 *
 * Utilizes the parse module to parse the command string
 * into a command and its arguments. 
 */
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

/**
 * @brief built in help function to list available commands
 *
 */
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
