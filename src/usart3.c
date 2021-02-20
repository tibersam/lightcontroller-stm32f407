#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include "usart3.h"

uint8_t usart3_tx[USART3_TX_BUFFER];
uint8_t usart3_rx[USART3_RX_BUFFER];


static volatile uint32_t tx_r_index;
static volatile uint32_t tx_w_index;
static volatile uint32_t rx_r_index;
static volatile uint32_t rx_w_index;


void usart3_gpio_configure(void);
void usart3_gpio_disable(void);


void usart3_gpio_configure(void)
{
	/*Set gpio*/
        gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10|GPIO11);
        gpio_set_af(GPIOB, GPIO_AF7, GPIO10|GPIO11);
}

void usart3_gpio_disable(void)
{
	gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO10|GPIO11);
}

void usart3_init(void)
{
	/* Enable the USART3 interrupt. */
	nvic_enable_irq(NVIC_USART3_IRQ);
 
	/*enable clocks for USART3*/
	rcc_periph_clock_enable(RCC_USART3);
	rcc_periph_clock_enable(RCC_GPIOB);

	/*Zero buffers*/
	for(int i = 0; i < USART3_TX_BUFFER; i++)
	{
		usart3_tx[i] = 0;
	}
	for(int i = 0; i < USART3_RX_BUFFER; i++)
	{
		usart3_rx[i] = 0;
	}
	
	tx_r_index = 0;
	tx_w_index = 0;
	rx_r_index = 0;
	rx_w_index = 0;


	/*Setup USART3*/
        usart_set_baudrate(USART3, 115200);
        usart_set_databits(USART3, 8);
        usart_set_stopbits(USART3, USART_STOPBITS_1);
        usart_set_mode(USART3, USART_MODE_TX_RX);   
        usart_set_parity(USART3, USART_PARITY_NONE);
        usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);
	/*enable uart*/
	usart_enable_rx_interrupt(USART3);
	usart3_enable();
}

void usart3_enable(void)
{
	usart3_gpio_configure();
	usart_enable(USART3);
	if(tx_w_index != tx_r_index)
	{
		usart_enable_tx_interrupt(USART3);
	}
}
void usart3_disable(void)
{
	usart_disable(USART3);
	usart3_gpio_disable();
}

/*
 * get current characters in rx buffer. Will be filled, if something is recieved. Concept is, main pulls periodically the buffer.
 * Content is coppied into s, l is lenght of buffer, so that an overfill is stoped
 */
uint8_t usart3_get_char(void)
{
	uint8_t ret = usart3_rx[rx_r_index];
	rx_r_index = (rx_r_index + 1) % USART3_RX_BUFFER;
	return ret;
}

unsigned int usart3_calc_rx_level(void)
{
	if(rx_w_index >= rx_r_index)
	{
		return rx_w_index - rx_r_index;
	}
	else
	{
		return USART3_RX_BUFFER - rx_r_index + rx_w_index;
	}
}

/*
 * Put characters into transmit buffer. They are then transmitted with interrupt support over rs232 interface
 */
void usart3_put_tx(char s[], int l)
{
	for(int i = 0; i < l; i++)
	{
		usart3_tx[tx_w_index] = (uint8_t) s[i];
		tx_w_index = (tx_w_index + 1) % USART3_TX_BUFFER;
	}
	usart_enable_tx_interrupt(USART3);
}

/*
 * Usart1 interrupt service routine. It copies bytes recieved to recieve buffer
 * It sends bytes in tx buffer. It uses 4 global counters for the two ringbuffers. 
 */
void usart3_isr(void)
{
	/* Check if we were called because of RXNE. */
        if (((USART_CR1(USART3) & USART_CR1_RXNEIE) != 0) &&
            ((USART_SR(USART3) & USART_SR_RXNE) != 0)) 
	{
		usart3_rx[rx_w_index] = usart_recv(USART3);
		rx_w_index = (rx_w_index + 1) % USART3_RX_BUFFER;
	}
	/*Check if we were called because of TXE*/
	if (((USART_CR1(USART3) & USART_CR1_TXEIE) != 0) &&
            ((USART_SR(USART3) & USART_SR_TXE) != 0)) 
	{
		usart_send(USART3, usart3_tx[tx_r_index]);
		tx_r_index = (tx_r_index + 1) % USART3_TX_BUFFER;
		if(tx_r_index == tx_w_index)
		{
			usart_disable_tx_interrupt(USART3);
		}
	}
}
