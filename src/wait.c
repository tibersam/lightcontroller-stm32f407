
#include "wait.h"
#include "consol.h"

volatile uint64_t tick;

void change_systick(int mode)
{
	systick_counter_disable();
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	if( mode == 0)
	{
		systick_set_reload(168000);
	}
	else
	{
		systick_set_reload(8000);
	}
	systick_clear();
	systick_counter_enable();
	systick_interrupt_enable();
}


void init_wait(void)
{
	tick = 0;
	change_systick(0);
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
