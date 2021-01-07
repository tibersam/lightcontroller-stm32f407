

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "button.h"
#include "atx.h"
#include "lightcontrol.h"
#include "consol.h"

void setup_gpio_button(void)
{
	rcc_periph_clock_enable(RCC_GPIOE);
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5|GPIO6);
	gpio_mode_setup(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO2|GPIO4);
	gpio_clear(GPIOE, GPIO5|GPIO6);	
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
}

void clear_led_button1(void)
{
	gpio_clear(GPIOE, GPIO5);
}

void set_led_button2(void)
{
	gpio_set(GPIOE, GPIO6);
}

void clear_led_button2(void)
{
	gpio_clear(GPIOE, GPIO6);
}

void process_button1(void)
{
	static int button1 = 0;
	static int disablecounter = 0; 
	if(disablecounter != 0)
	{
		disablecounter++;
		if(disablecounter == 260)
		{
			disable_atx();
			disablecounter = 0;
			clear_led_button1();
		}
		if(disablecounter == 300)
		{
			disablecounter = 0;
			clear_led_button1();
		}
	}
	if(button1 != get_button1())
	{
		button1 = get_button1();
		if((button1 == 1)&&(disablecounter == 0))
		{
			if(get_atx_status() == 0)
			{
				enable_atx();
				set_stepmode(1);
				disablecounter = 261;
				setrgbvalues(255,255,255);
			}
			else
			{
				set_stepmode(0);
				disablecounter = 1;
				setrgbvalues(0,0,0);
			}
			print("[Button1]: pressed\n");
			set_led_button1();
		}
	}
}

void process_button(void)
{
	process_button1();
}
