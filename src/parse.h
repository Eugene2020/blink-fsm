#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum { CMD_NONE, CMD_BLINK, CMD_STOP } cmd_t;

typedef struct {
    cmd_t type;
    uint16_t count;   
    uint16_t period_ms; 
} command_t;

bool parse_feed(const char *line, command_t *out);