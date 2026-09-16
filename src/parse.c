#include "parse.h"
#include <string.h>
#include <stdlib.h>

/* commands */
bool parse_feed(const char *line, command_t *out) {
    out->type = CMD_NONE;

    if (strncmp(line, "stop", 4) == 0) {
        out->type = CMD_STOP;
        return true;
    }

    char *end;
    long n = strtol(line, &end, 10);
    if (end == line) return false;
    while (*end == ' ') end++;
    long t = strtol(end, &end, 10);
    if (n <= 0 || n > 1000) return false;
    if (t <= 0 || t > 60000) return false;

    out->type = CMD_BLINK;
    out->count = (uint16_t)n;
    out->period_ms = (uint16_t)t;
    return true;
}