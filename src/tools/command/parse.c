#include "parse.h"

static bool convert_arg(char* arg, long* value)
{
    char* endptr;

    // convert both hex and decimal strings
    if (strlen(arg) > 2 && arg[0] == '0' && 
        (arg[1] == 'x' || arg[1] == 'X')) {
        *value = strtol(arg, &endptr, 16);
    } else {
        *value = strtol(arg, &endptr, 10);
    }
    
    // return true if parsing was successful
    return (endptr != arg && *endptr == '\0');

}

uint8_t parse_line(char* cmd_line, char* cmd, int32_t args[])
{
    uint8_t count = 0;
    char* token;
    char* saveptr;
    
    token = strtok_r(cmd_line, " ", &saveptr);

    // save first token in cmd_line
    if (token != NULL)
    {
        strcpy(cmd, token);
    }
    
    token = strtok_r(NULL, " ", &saveptr);

    // get the args
    while (token != NULL) 
    {
        long value;
        if (convert_arg(token, &value)) 
        {
            args[count++] = (int32_t)value;
        } 
        else 
        {
            printf("WARNING: Could not parse '%s' as integer\n", token);
        }
        
        token = strtok_r(NULL, " ", &saveptr);
    }
    
    return count;
}
