#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "command_interface.h"
#include "parse.h"

// Command table
static const cmd_entry_t* command_table[MAX_COMMANDS];
static uint8_t command_count = 0;

// Builtin help handler
void help_handler(const int32_t* args);
static cmd_entry_t help_command = {
    .name = "help",
    .handler = help_handler,
    .num_args = 0,
};

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
static void cmd_execute(parsed_cmd_t* parsed_cmd)
{
    // search command table for a match
    for (uint8_t i = 0; i < command_count; i++) 
    {
        if (strcmp(parsed_cmd->cmd, command_table[i]->name) == 0) 
        {
            // Validate argument count
            if (parsed_cmd->num_args != command_table[i]->num_args) 
            {
                printf("\nERROR: Command '%s' expects %d arguments, got %d\n",
                       command_table[i]->name,
                       command_table[i]->num_args,
                       parsed_cmd->num_args);
                return;
            }
            
            // Execute command
            command_table[i]->handler(parsed_cmd->args);
            return;
        }
    }
    
    // Unknown command
    printf("\nERROR: Unknown command '%s'. Type 'help' for available commands.\n", 
           parsed_cmd->cmd);
}


/**
 * @brief init the command table with the help command
 */
void cmd_init(void)
{
    // register built in help command
    cmd_register(&help_command);
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

            parsed_cmd_t parsed_cmd;
            if (parse_line(cmd_line, &parsed_cmd))
            {
                cmd_execute(&parsed_cmd);
            }
            return;
        }

        cmd_line[cmd_line_pos++] = c;
    }
}

/**
 * @brief built in help function to list available commands
 *
 */
void help_handler(const int32_t* args)
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

