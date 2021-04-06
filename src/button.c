

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "button.h"
#include "atx.h"
#include "lightcontrol.h"
#include "consol.h"
#include "button_consol.h"
#include "wait.h"
#include "timeout_module.h"

static int button1 = 0;
static uint64_t button1_tick = 0;
static int disablecounter = 0;

void setup_gpio_button(void)
{
	rcc_periph_clock_enable(RCC_GPIOE);
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5|GPIO6);
	gpio_mode_setup(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO2|GPIO4);
	gpio_clear(GPIOE, GPIO5|GPIO6);
	button1 = 0;
	disablecounter = 0;
}

int get_white(void)
{
	return button1;
}

uint64_t get_white_tick(void)
{
	return button1_tick;
}

int get_button1(void)
{
	if(gpio_get(GPIOE, GPIO4) == 0)
	{
		return 1;
	}
	return 0;
}

int get_button2(void)
{
	if(gpio_get(GPIOE, GPIO2) == 0)
	{
		return 1;
	}
	return 0;
}

void set_led_button1(void)
{
	gpio_set(GPIOE, GPIO5);
	button_box_puts("set white 1\n");

}

void clear_led_button1(void)
{
	gpio_clear(GPIOE, GPIO5);
	button_box_puts("set white 0\n");
}

void set_led_button2(void)
{
	gpio_set(GPIOE, GPIO6);
}

void clear_led_button2(void)
{
	gpio_clear(GPIOE, GPIO6);
}

void process_atx_switch(int mode)
{
	static float hue = 0.0;
	static float sat = 0.0;
	static float intens = 0.0;
	if(disablecounter != 0)
	{
		disablecounter++;
		if(disablecounter == 260)
		{
			clear_led_button1();
			wait(2);
			disable_atx();
			disablecounter = 0;
		}
		if(disablecounter == 300)
		{
			disablecounter = 0;
			clear_led_button1();
		}
		if((disablecounter == 2) || (disablecounter == 280))
		{
			set_led_button1();
		}
	}
	if( mode == 1 )
	{
		get_hsi( &hue, &sat, &intens);
		int stepmode = get_stepmode();
		set_stepmode(0);
		disablecounter = 1;
		setcycelhsi( hue, sat, 0.0, 256);
		set_led_button1();
		set_stepmode(stepmode);
	}
	if( mode == 2)
	{
		enable_atx();
		int stepmode = get_stepmode();
		set_stepmode(1);
		int wait = get_waitlength();
		set_waitlength(4);
		disablecounter = 261;
		setcycelhsi(hue, sat, intens, 256);
		set_waitlength(wait);
		set_stepmode(stepmode);
	}
}

void process_button1(void)
{
	int change = 0;
	process_atx_switch(0);
	if(button1 != get_button1())
	{
		change = 1;
		button1 = get_button1();
		if((button1 == 1) && (disablecounter == 0))
		{
			set_led_button1();
		}	
		else
		{
			clear_led_button1();
		}
		reset_timeout();
	}
	if((disablecounter == 0) && (change == 1))
	{
		if(get_atx_status() == 0)
		{
			if(button1 == 1)
			{
				process_atx_switch(2);
				print("[Button1]: pressed\n");
			}
		}
		else
		{
			if(button1 == 0 && get_tick() - button1_tick < 1000)
			{
				process_atx_switch(1);
				print("[Button1]: pressed\n");
			}
		}
	}
	if(change == 1)
	{
		button1_tick = get_tick();
	}
}

void process_button(void)
{
	process_button1();
}
