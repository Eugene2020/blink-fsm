#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>

#include "uart.h"
#include "fsm.h"

static volatile uint32_t tick_ms = 0;

void sys_tick_handler(void) { tick_ms++; }

static void clock_setup(void) {
    /* Blue Pill: 8 MHz HSE * 9 = 72 MHz */
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
}

static void systick_setup(void) {
    /* 1 ms 72 MHz */
    systick_set_frequency(1000, 72000000);
    systick_counter_enable();
    systick_interrupt_enable();
}

int main(void) {
    clock_setup();
    uart_setup();
    fsm_init();
    systick_setup();

    uart_puts("\r\nBlink-FSM ready. Commands: '<N> <ms>' or 'stop'\r\n");

    while (1) {
        fsm_poll_input();

        static uint32_t last = 0;
        if (tick_ms != last) {
            last = tick_ms;
            fsm_tick_1ms();
        }
    }
}