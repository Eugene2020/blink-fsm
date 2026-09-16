#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "parse.h"

typedef enum { ST_IDLE, ST_BLINK_ON, ST_BLINK_OFF } state_t;

void fsm_init(void);
void fsm_handle_command(const command_t *cmd);
void fsm_tick_1ms(void);
void fsm_poll_input(void);