#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include "usart1.h"

uint8_t usart1_tx[USART1_TX_BUFFER];
uint8_t usart1_rx[USART1_RX_BUFFER];


static volatile uint32_t tx_r_index;
static volatile uint32_t tx_w_index;
static volatile uint32_t rx_r_index;
static volatile uint32_t rx_w_index;


void usart1_gpio_configure(void);
void usart1_gpio_disable(void);


void usart1_gpio_configure(void)
{
	/*Set gpio*/
        gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9|GPIO10);
        gpio_set_af(GPIOA, GPIO_AF7, GPIO9|GPIO10);
}

void usart1_gpio_disable(void)
{
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO9|GPIO10);
}

void usart1_init(void)
{
	/* Enable the USART5 interrupt. */
	nvic_enable_irq(NVIC_USART1_IRQ);
 
	/*enable clocks for USART5*/
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_GPIOA);

	/*Zero buffers*/
	for(int i = 0; i < USART1_TX_BUFFER; i++)
	{
		usart1_tx[i] = 0;
	}
	for(int i = 0; i < USART1_RX_BUFFER; i++)
	{
		usart1_rx[i] = 0;
	}
	
	tx_r_index = 0;
	tx_w_index = 0;
	rx_r_index = 0;
	rx_w_index = 0;


	/*Setup USART5*/
        usart_set_baudrate(USART1, 115200);
        usart_set_databits(USART1, 8);
        usart_set_stopbits(USART1, USART_STOPBITS_1);
        usart_set_mode(USART1, USART_MODE_TX_RX);   
        usart_set_parity(USART1, USART_PARITY_NONE);
        usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	/*enable uart*/
	usart_enable_rx_interrupt(USART1);
	usart1_enable();
}

void usart1_enable(void)
{
	usart1_gpio_configure();
	usart_enable(USART1);
}
void usart1_disable(void)
{
	usart_disable(USART1);
	usart1_gpio_disable();
}

void usart1_recalc_baudrate(void)
{
        usart_set_baudrate(USART1, 115200);
}

/*
 * get current characters in rx buffer. Will be filled, if something is recieved. Concept is, main pulls periodically the buffer.
 * Content is coppied into s, l is lenght of buffer, so that an overfill is stoped
 */
uint8_t usart1_get_char(void)
{
	uint8_t ret = usart1_rx[rx_r_index];
	rx_r_index = (rx_r_index + 1) % USART1_RX_BUFFER;
	return ret;
}

unsigned int usart1_calc_rx_level(void)
{
	if(rx_w_index >= rx_r_index)
	{
		return rx_w_index - rx_r_index;
	}
	else
	{
		return USART1_RX_BUFFER - rx_r_index + rx_w_index;
	}
}

/*
 * Put characters into transmit buffer. They are then transmitted with interrupt support over rs232 interface
 */
void usart1_put_tx(char s[], int l)
{
	for(int i = 0; i < l; i++)
	{
		usart1_tx[tx_w_index] = (uint8_t) s[i];
		tx_w_index = (tx_w_index + 1) % USART1_TX_BUFFER;
	}
	usart_enable_tx_interrupt(USART1);
}

/*
 * Usart1 interrupt service routine. It copies bytes recieved to recieve buffer
 * It sends bytes in tx buffer. It uses 4 global counters for the two ringbuffers. 
 */
void usart1_isr(void)
{
	/* Check if we were called because of RXNE. */
        if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
            ((USART_SR(USART1) & USART_SR_RXNE) != 0)) 
	{
		usart1_rx[rx_w_index] = usart_recv(USART1);
		rx_w_index = (rx_w_index + 1) % USART1_RX_BUFFER;
	}
	/*Check if we were called because of TXE*/
	if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) &&
            ((USART_SR(USART1) & USART_SR_TXE) != 0)) 
	{
		usart_send(USART1, usart1_tx[tx_r_index]);
		tx_r_index = (tx_r_index + 1) % USART1_TX_BUFFER;
		if(tx_r_index == tx_w_index)
		{
			usart_disable_tx_interrupt(USART1);
		}
	}
}
