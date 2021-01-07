#ifndef USART1_TX_RX_RINGBUFFER
#define USART1_TX_RX_RINGBUFFER

#define USART1_TX_BUFFER 1000
#define USART1_RX_BUFFER 50


#include <stdint.h>

/**
 * usart1_init
 *
 * This zeros all ringbuffers and initialises the usart1
 */
void usart1_init(void);

/**
 * usart1_enable
 *
 * enabel usart1 hardware and gpio
 */
void usart1_enable(void);

/**
 * usart1_disable
 *
 * disabel usart1 hardware and set gpio to input
 */
void usart1_disable(void);

/**
 * usart5_get_char
 *
 * get char from ringbuffer, does not care about fill level 
 */
uint8_t usart1_get_char(void);

/**
 * usart1_calc_rx_level
 *
 * get filllevel of rx ringbuffer.
 * Indicates number of chars in buffer.
 */
unsigned int usart1_calc_rx_level(void);

/**
 * usart1_put_tx
 *
 * coppy string to tx ringbuffer. Enable interrupt to force send
 */
void usart1_put_tx(char s[], int l);

#endif
