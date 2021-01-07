
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include "uart5.h"


uint8_t usart5_tx[USART5_TX_BUFFER];
uint8_t usart5_rx[USART5_RX_BUFFER];

static volatile uint32_t tx_r_index;
static volatile uint32_t tx_w_index;
static volatile uint32_t rx_r_index;
static volatile uint32_t rx_w_index;


void usart5_gpio_configure(void);
void usart5_gpio_disable(void);

void usart5_gpio_configure(void)
{
	/*Set gpio*/
        gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
        gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
        gpio_set_af(GPIOD, GPIO_AF8, GPIO2);
        gpio_set_af(GPIOC, GPIO_AF8, GPIO12);
}
void usart5_gpio_disable(void)
{
	gpio_mode_setup(GPIOD, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO2);
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO12);
}

void usart5_init(void)
{
	/* Enable the USART5 interrupt. */
	nvic_enable_irq(NVIC_UART5_IRQ);
 
	/*enable clocks for USART5*/
	rcc_periph_clock_enable(RCC_UART5);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOC);

	/*Zero buffers*/
	for(int i = 0; i < USART5_TX_BUFFER; i++)
	{
		usart5_tx[i] = 0;
	}
	for(int i = 0; i < USART5_RX_BUFFER; i++)
	{
		usart5_rx[i] = 0;
	}
	
	tx_r_index = 0;
	tx_w_index = 0;
	rx_r_index = 0;
	rx_w_index = 0;


	/*Setup USART5*/
        usart_set_baudrate(UART5, 115200);
        usart_set_databits(UART5, 8);
        usart_set_stopbits(UART5, USART_STOPBITS_1);
        usart_set_mode(UART5, USART_MODE_TX_RX);   
        usart_set_parity(UART5, USART_PARITY_NONE);
        usart_set_flow_control(UART5, USART_FLOWCONTROL_NONE);
	/*enable uart*/
	usart_enable_rx_interrupt(UART5);
	usart5_enable();
}

void usart5_enable(void)
{
	usart5_gpio_configure();
	usart_enable(UART5);
}
void usart5_disable(void)
{
	usart_disable(UART5);
	usart5_gpio_disable();
}

/*
 * get current characters in rx buffer. Will be filled, if something is recieved. Concept is, main pulls periodically the buffer.
 * Content is coppied into s, l is lenght of buffer, so that an overfill is stoped
 */
uint8_t usart5_get_char(void)
{
	uint8_t ret = usart5_rx[rx_r_index];
	rx_r_index = (rx_r_index + 1) % USART5_RX_BUFFER;
	return ret;
}

unsigned int uart5_calc_rx_level(void)
{
	if(rx_w_index >= rx_r_index)
	{
		return rx_w_index - rx_r_index;
	}
	else
	{
		return USART5_RX_BUFFER - rx_r_index + rx_w_index;
	}
}

/*
 * Put characters into transmit buffer. They are then transmitted with interrupt support over rs232 interface
 */
void usart5_put_tx(char s[], int l)
{
	for(int i = 0; i < l; i++)
	{
		usart5_tx[tx_w_index] = (uint8_t) s[i];
		tx_w_index = (tx_w_index + 1) % USART5_TX_BUFFER;
	}
	usart_enable_tx_interrupt(UART5);
}

/*
 * Usart5 interrupt service routine. It copies bytes recieved to recieve buffer
 * It sends bytes in tx buffer. It uses 4 global counters for the two ringbuffers. 
 */
void uart5_isr(void)
{
	/* Check if we were called because of RXNE. */
        if (((USART_CR1(UART5) & USART_CR1_RXNEIE) != 0) &&
            ((USART_SR(UART5) & USART_SR_RXNE) != 0)) {
		usart5_rx[rx_w_index] = usart_recv(UART5);
		rx_w_index = (rx_w_index + 1) % USART5_RX_BUFFER;
	}
	/*Check if we were called because of TXE*/
	if (((USART_CR1(UART5) & USART_CR1_TXEIE) != 0) &&
            ((USART_SR(UART5) & USART_SR_TXE) != 0)) {
		usart_send(UART5, usart5_tx[tx_r_index]);
		tx_r_index = (tx_r_index + 1) % USART5_TX_BUFFER;
		if(tx_r_index == tx_w_index)
		{
			usart_disable_tx_interrupt(UART5);
		}
	}
}
