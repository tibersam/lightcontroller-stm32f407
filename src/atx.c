#include "atx.h"

#include "wait.h"
#include "lightcontrol.h"
#include "uart5.h"
#include "consol.h"
#include "usart3.h"


void setup_gpio_atx(void)
{
	rcc_periph_clock_enable(RCC_GPIOE);
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1|GPIO3);
	gpio_mode_setup(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
	gpio_clear(GPIOE, GPIO1|GPIO3);	
}

int get_atx_status(void)
{
	if(gpio_get(GPIOE, GPIO0) == 0)
	{
		return 0;
	}
	return 1;
}

void enable_atx(void)
{
	if(gpio_get(GPIOE, GPIO0) != 0)
	{
		return;
	}
	//Enable atx
	gpio_set(GPIOE, GPIO1);
	print("[ATX] Send endable signal\n");
	for(int i = 0; i < NUMBERLED; i++)
	{
		setLEDrgbhsi(i, 0, 0, 0);
	}
	setrgbvalues(0,0,0);	
	calculatestep();
	preparebuffer();
	gpio_set(GPIOE,GPIO3);
	sendbuffer();
	usart3_enable();
	//Check weather atx ready signal provided
	while(gpio_get(GPIOE, GPIO0) ==0)
	{
		//check_uart();
		__asm__("nop");
	//	wait(1);
	}
	usart5_enable();
	print("[ATX] Atx on\n");
}

void disable_atx(void)
{
	if(gpio_get(GPIOE, GPIO0) == 0)
	{
		return;
	}
	print("[ATX]: remove enable signal\n ");
	//disbale spi ws2812 drivers
	gpio_clear(GPIOE, GPIO3);
	usart5_disable();
	usart3_disable();
	//disable atx 
	gpio_clear(GPIOE, GPIO1);
	print("[ATX]: Atx off\n");
}

