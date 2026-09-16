#include "fsm.h"
#include "uart.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdio.h>
#include <string.h>

#define LED_PORT GPIOC
#define LED_PIN  GPIO13

/* state */
static state_t state = ST_IDLE;

static uint16_t remaining;      
static uint16_t half_period_ms; 
static uint32_t counter_ms;     

static uint32_t blinks_done;    

/* buffer */
static char line[32];
static uint8_t line_len = 0;

/* LED */
static inline void led_on(void)  { gpio_clear(LED_PORT, LED_PIN); }
static inline void led_off(void) { gpio_set(LED_PORT, LED_PIN); }

static const char *state_name(state_t s) {
    switch (s) {
        case ST_IDLE:      return "IDLE";
        case ST_BLINK_ON:  return "BLINK_ON";
        case ST_BLINK_OFF: return "BLINK_OFF";
        default:           return "UNKNOWN";
    }
}

void fsm_init(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(LED_PORT, GPIO_MODE_OUTPUT_2_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, LED_PIN);
    led_off();
}

static void report_status(void) {
    char buf[64];
    snprintf(buf, sizeof buf, "[done] blinks=%lu state=%s\r\n",
             (unsigned long)blinks_done, state_name(state));
    uart_puts(buf);
}

void fsm_handle_command(const command_t *cmd) {
    switch (cmd->type) {
    case CMD_BLINK:
        led_off();
        blinks_done = 0;
        remaining   = cmd->count;
        half_period_ms = cmd->period_ms;
        counter_ms  = 0;
        led_on();
        state = ST_BLINK_ON;
        uart_puts("[cmd] start\r\n");
        break;

    case CMD_STOP:
        led_off();
        state = ST_IDLE;
        report_status();
        break;

    default: break;
    }
}

void fsm_tick_1ms(void) {
    switch (state) {
    case ST_IDLE:
        break;

    case ST_BLINK_ON:
        if (++counter_ms >= half_period_ms) {
            counter_ms = 0;
            led_off();
            state = ST_BLINK_OFF;
        }
        break;

    case ST_BLINK_OFF:
        if (++counter_ms >= half_period_ms) {
            counter_ms = 0;
            blinks_done++;
            remaining--;

            char buf[32];
            snprintf(buf, sizeof buf, "blink %lu\r\n",
                     (unsigned long)blinks_done);
            uart_puts(buf);

            if (remaining == 0) {
                state = ST_IDLE;
                report_status();
            } else {
                led_on();
                state = ST_BLINK_ON;
            }
        }
        break;
    }
}

void fsm_start_first_blink(void) {
    if (state == ST_BLINK_ON) led_on();
}

void fsm_poll_input(void) {
    uint8_t c;
    while (uart_getc(&c)) {
        if (c == '\r' || c == '\n') {
            if (line_len == 0) continue;
            line[line_len] = '\0';
            command_t cmd;
            if (parse_feed(line, &cmd) && cmd.type != CMD_NONE) {
                fsm_handle_command(&cmd);
            } else {
                uart_puts("[err] bad command\r\n");
            }
            line_len = 0;
        } else if (c >= 32 && c <= 126) {
            line[line_len++] = (char)c;
        } else {
            line_len = 0; 
        }
    }
}