#include "atx.h"

#include "wait.h"
#include "lightcontrol.h"
#include "uart5.h"
#include "consol.h"
#include "usart3.h"
#include "usart1.h"
#include <libopencm3/stm32/flash.h>


// configure to 8 MHZ
const struct rcc_clock_scale my_fallback_clock = {
	        .pllm = 16,
                .plln = 96,
                .pllp = 6,  
                .pllq = 0,  
                .pllr = 0,  
                .pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
                .hpre = RCC_CFGR_HPRE_DIV_NONE,
                .ppre1 = RCC_CFGR_PPRE_DIV_NONE,  
                .ppre2 = RCC_CFGR_PPRE_DIV_NONE,
                .voltage_scale = PWR_SCALE2,    
                .flash_config = FLASH_ACR_DCEN | FLASH_ACR_ICEN |
                                FLASH_ACR_LATENCY_0WS,
                .ahb_frequency  = 8000000,
                .apb1_frequency = 8000000,
                .apb2_frequency = 8000000,
};

void change_system_clock(int mode)
{
	usart1_disable();
	if(mode == 1)
	{
		rcc_clock_setup_pll(&my_fallback_clock);
		change_systick(1);
	}
	else
	{
		rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
		change_systick(0);
	}
	usart1_recalc_baudrate();
	usart1_enable();
}

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
	if(rcc_ahb_frequency != 164000000)
	{
		change_system_clock(0);
	}
	print("[ATX] Send endable signal\n");
	for(int i = 0; i < NUMBERLED; i++)
	{
		setLEDrgbw(i, 0, 0, 0, 0);
	}
	preparebuffer();
	force_update_led();
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
	print("[OK]: Atx on\n");
}

void disable_atx(void)
{
	if(gpio_get(GPIOE, GPIO0) == 0)
	{
		return;
	}
	//disbale spi ws2812 drivers
	gpio_clear(GPIOE, GPIO3);
	usart5_disable();
	usart3_disable();
	if(rcc_ahb_frequency != 8000000)
	{
		change_system_clock(1);
	}
	print("[OK]: remove enable signal\n ");
	//disable atx 
	gpio_clear(GPIOE, GPIO1);
	print("[ATX]: Atx off\n");
}

