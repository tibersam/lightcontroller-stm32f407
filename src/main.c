

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#include "wait.h"
#include "atx.h"
#include "lightcontrol.h"
#include "uart5.h"
#include "consol.h"
#include "button.h"
#include "usart1.h"

void init(void);

static void clock_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6|GPIO7);
	gpio_clear(GPIOA, GPIO6|GPIO7);
	gpio_set(GPIOA, GPIO6);

	

	gpio_clear(GPIOA, GPIO6);

	rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
	//gpio_set(GPIOA, GPIO7);
}


void init(void)
{
	clock_setup();
	init_wait();
	initledmodule();
	setuplightcontroller();
	setup_gpio_atx();
	usart5_init();
	usart1_init();
	consol_init();
	setup_gpio_button();
}

int main(void)
{

	init();
	//disable_atx();
	gpio_toggle(GPIOA, GPIO6);
	int i = 0;
	while(1)
	{
		wait(100);
		gpio_toggle(GPIOA, GPIO6);
		i++;
		if(i == 20)
		{
			break;
		}

	}
	i = 0;
	enable_atx();
	print("Welcome to Lightcontroller Version 2.0\n");
	print("Testing ATX\n");
	gpio_set(GPIOA, GPIO6);
	gpio_clear(GPIOA, GPIO7);
	for(i = 0; i < 6; i++)
	{
		wait(500);
		gpio_toggle(GPIOA, GPIO6|GPIO7);
	}
	wait(200);
	disable_atx();
	gpio_clear(GPIOA, GPIO6|GPIO7);
	for(i = 0; i < 6; i++)
	{
		wait(700);
		gpio_toggle(GPIOA, GPIO6|GPIO7);
	}
	enable_atx();
	print("[READY] Selftest complete\n");
	print("Enable Consol\n");
	uint64_t last_tick = get_tick();
	setrgbvalues(255, 255, 255);
	int j = 0;
	while(1 == 1)
	{
	/*	switch(j)
		{
		case 0:
			setrgbvalues(255,0,0);
			break;
		case 1:
			setrgbvalues(0,255,0);
			break;
		case 2:
			setrgbvalues(0,0,255);
			break;
		case 3:
			setrgbvalues(255,255,255);
			break;
		case 4:
			setrgbvalues(255,128,128);
			break;
		case 5:
			setrgbvalues(64,0,64);
			break;
		case 6:
			setrgbvalues(128,128,64);
			break;
		case 7:
			setrgbvalues(2,200,150);
			break;
		case 8:
			setrgbvalues(10,60,50);
			break;
		case 9:
			setrgbvalues(127, 43, 183);
			break;
		case 10:
			setrgbvalues(12,210,111);
			break;
		case 11:
			setrgbvalues(236,255,107);
			break;
		case 12:
			setrgbvalues(1,1,1);
			break;
		case 13:
			setrgbvalues(30,0,30);
			break;
		case 14:
			setrgbvalues(0,0,0);
			break;
		default:
			j = 100;
			break;
		}
		if(j == 100)
		{
			break;
		}
		j++;*/
		//for(i = 0; i < 1850; i++)
		//{
			last_tick = get_tick();
			if(get_atx_status() == 1)
			{
				sendbuffer();
				calculatestep();
				preparebuffer();
			}
			gpio_toggle(GPIOA, GPIO6|GPIO7);
			check_uart();
			process_button();
			if(get_atx_status() == 0)
			{
				wait_until(last_tick + 1000);
			}
			wait_until(last_tick + 20);
			//usart5_put_tx("asdf\r",5);
		//}
	}
	disable_atx();
	gpio_set(GPIOA, GPIO6);
	gpio_clear(GPIOA, GPIO7);
	while(1)
	{
		gpio_toggle(GPIOA, GPIO6|GPIO7);
		wait(200);
	}

}

