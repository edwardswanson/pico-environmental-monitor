#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define CMD_MAX_ARGS 16
#define CMD_MAX_LEN 16

typedef struct
{
    char cmd[CMD_MAX_LEN];
    int32_t args[CMD_MAX_ARGS];
    uint8_t num_args;
    bool valid;
} parsed_cmd_t;

bool parse_line(char* cmd_line, parsed_cmd_t* parsed_cmd);


