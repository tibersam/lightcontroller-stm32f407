#ifndef USART3_TX_RX_RINGBUFFER
#define USART3_TX_RX_RINGBUFFER

#define USART3_TX_BUFFER 100
#define USART3_RX_BUFFER 50


#include <stdint.h>

/**
 * usart3_init
 *
 * This zeros all ringbuffers and initialises the usart1
 */
void usart3_init(void);

/**
 * usart3_enable
 *
 * enabel usart3 hardware and gpio
 */
void usart3_enable(void);

/**
 * usart3_disable
 *
 * disabel usart3 hardware and set gpio to input
 */
void usart3_disable(void);

/**
 * usart3_get_char
 *
 * get char from ringbuffer, does not care about fill level 
 */
uint8_t usart3_get_char(void);

/**
 * usart3_calc_rx_level
 *
 * get filllevel of rx ringbuffer.
 * Indicates number of chars in buffer.
 */
unsigned int usart3_calc_rx_level(void);

/**
 * usart3_put_tx
 *
 * coppy string to tx ringbuffer. Enable interrupt to force send
 */
void usart3_put_tx(char s[], int l);

#endif
