#ifndef USART5_TX_RX_RINGBUFFER
#define USART5_TX_RX_RINGBUFFER

#define USART5_TX_BUFFER 500
#define USART5_RX_BUFFER 50

#include <stdint.h>
/*
 * usart5_init
 *
 * This zeros all ringbuffers for uart5, initialises all global variables needed
 * Confiugres GPIO and USART5 with interrupt support
 */
void usart5_init(void);

void usart5_enable(void);

void usart5_disable(void);


/*
 * usart5_get_rx
 *
 * This copies content from the RX buffer to the string s provided by the caller.
 * l indicates the lenght of the string
 */
uint8_t usart5_get_char(void);

/*
 * uart5_calc_rx_level
 *
 * Calculate the fillelvel of the rx buffer
 */
unsigned int uart5_calc_rx_level(void);

/*
 * usart5_pug_tx
 *
 * This copies the stirng s to the TX buffer, that then will be send using isr
 *
 *
 */
void usart5_put_tx(char s[], int l);

#endif 
