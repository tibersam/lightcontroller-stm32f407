
#include "timeout_module.h"
#include "wait.h"
#include "atx.h"
#include "button.h"
#include "consol.h"

static uint64_t last_update;
static uint64_t timeout_time;

void init_timeout_module(void)
{
	last_update = 0;
	timeout_time = 14400000;
}

void reset_timeout(void)
{
	last_update = get_tick();
}

void set_timeouttime(uint64_t timeout)
{
	timeout_time = timeout;
}

void check_timeout(void)
{
	static int mode = 0;
	if(get_atx_status() == 0)
	{
		return;
	}
	if((last_update + timeout_time) <= get_tick())
	{
		if(mode == 0)
		{
			mode = 1;
			process_atx_switch(1);
			print("[TIMEOUT]: Timeout reached. Now shutting down lights and atx\n");
			return;
		}
	}
	else
	{
		mode = 0;
	}
}
