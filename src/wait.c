
#include "wait.h"

volatile uint64_t tick;

void init_wait(void)
{
	tick = 0;
	systick_counter_disable();
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_reload(168000);
	systick_clear();
	systick_counter_enable();
	systick_interrupt_enable();
}

uint64_t get_tick(void)
{
	return tick;
}

void wait_until(uint64_t timestamp)
{
	while(timestamp > tick)
	{
		__asm__("nop");
	}
}

void wait(uint64_t ms)
{
	uint64_t tickms = tick + ms;
	if(tick > tickms)
	{
		tick = 0;
		tickms = ms;
	}
	wait_until(tickms);
}

void sys_tick_handler(void)
{
  tick++;
}
