

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
#include "usart3.h"
#include "button_consol.h"
#include "timeout_module.h"
#include "decoder.h"

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
	usart3_init();
	usart1_init();
	consol_init();
	setup_gpio_button();
	button_decoder_init();
	init_timeout_module();
}

int main(void)
{

	init();
	//disable_atx();
	gpio_toggle(GPIOA, GPIO6);
	for(int i = 0; i < 20; i++)
	{
		wait(100);
		gpio_toggle(GPIOA, GPIO6);
	}
	enable_atx();
	print("Welcome to Lightcontroller Version "SOFTWAREVERSION"\n");
	print("Testing ATX\n");
	print("Testing Buttons\n");
	button_test();
	gpio_set(GPIOA, GPIO6);
	gpio_clear(GPIOA, GPIO7);
	for(int i = 0; i < 6; i++)
	{
		check_uart();
		wait(100);
		gpio_toggle(GPIOA, GPIO6|GPIO7);
	}
	wait(200);
	disable_atx();
	gpio_clear(GPIOA, GPIO6|GPIO7);
	for(int i = 0; i < 2; i++)
	{
		wait(500);
		gpio_toggle(GPIOA, GPIO6|GPIO7);
	}
	enable_atx();
	print("[READY]: Selftest complete\n");
	print("[READY]: Enable Consol\n");
	uint64_t last_tick = 0;
	check_uart();
	check_buttons();
	set_stepmode(1);
	setrgbvalues(255, 255, 255);
	set_waitlength(4);
	reset_timeout();
	enable_uart5_consol();
	print("[START]: Start main loop\n");
	while(1 == 1)
	{
		last_tick = get_tick();
		if(get_atx_status() == 1)
		{
			gpio_clear(GPIOA, GPIO6|GPIO7);
			sendbuffer();
			calculatestep();
			preparebuffer();
			check_buttons();
		}
		check_uart();
		process_button();
		check_timeout();
		if(get_atx_status() == 0)
		{
			while(get_tick() < last_tick + 10000)
			{
				if(get_button1() == 1)
					break;
				if(usart1_calc_rx_level() != 0)
					break;
				wait(70);
				gpio_toggle(GPIOA, GPIO6);
			}
		}
		gpio_toggle(GPIOA, GPIO6|GPIO7);
		wait_until(last_tick + 20);
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

