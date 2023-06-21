#include "delay.h"

void delay_ms(volatile uint32_t ms)
{ // 1ms real time delay
	board_delay_ms(ms, OSP_DELAY_OS_COMPAT_DISABLE);
}

void delay_s(volatile uint32_t s)
{ // 1s real time delay
	board_delay_ms(1000 * s, OSP_DELAY_OS_COMPAT_DISABLE);
}

void delay_us(uint32_t us)
{
	uint64_t start_us, us_delayed;

	us_delayed = (uint64_t)us;
	start_us = board_get_cur_us();
	while ((board_get_cur_us() - start_us) < us_delayed);
}