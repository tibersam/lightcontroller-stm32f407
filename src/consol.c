#include "consol.h"
#include "uart5.h"
#include "decoder.h"
#include "usart1.h"

uint8_t uart5_buffer[UART5_BUFFER];
uint32_t uart5_level; 
uint8_t uart5_echo;
uint8_t uart5_consol;

uint8_t usart1_buffer[USART1_BUFFER];
uint32_t usart1_level;
uint8_t usart1_echo;

void check_uart5(void);
void check_usart1(void);
void clearup_consol(void);

void consol_init(void)
{
	for(int i = 0; i < UART5_BUFFER; i++)
	{
		uart5_buffer[i] = 0;
	}
	uart5_level = 0;
	uart5_echo = 1;
	uart5_consol = 1;
	for(int i = 0; i < USART1_BUFFER; i++)
	{
		usart1_buffer[i] = 0;
	}
	usart1_level = 0;
	usart1_echo = 1;
}

void set_echo(int echo)
{
	usart1_echo = echo;
	if(uart5_consol == 1)
	{
		uart5_echo = echo;
	}
}

void enable_uart5_consol(void)
{
	uart5_echo = usart1_echo;
	uart5_consol = 1;
}

void disable_uart5_consol(void)
{
	uart5_echo = 0;
	uart5_consol = 0;
}


void check_uart(void)
{
	if(uart5_consol == 1)
	{
		check_uart5();
	}
	check_usart1();
}

void check_usart1(void)
{
	while(usart1_calc_rx_level() != 0)
	{
		usart1_buffer[usart1_level] = usart1_get_char();
		if(usart1_echo == 1)
		{
			if((usart1_buffer[usart1_level] == '\010') || (usart1_buffer[usart1_level] == '\127') || (usart1_buffer[usart1_level] == '\177'))
			{
				usart1_put_tx("\010 \010",3);
				if(usart1_level > 0)
				{
					usart1_level--;
					if(usart1_level > 0)
					{
						usart1_level--;
					}
				}
			}
			else
			{
				usart1_put_tx((char *)&(usart1_buffer[usart1_level]), 1);
			}
		}
		if(usart1_buffer[usart1_level] == '\r')
		{
			usart1_buffer[usart1_level] = '\000';
			if(usart1_echo)
			{
				print("\n");
			}
			//Recieved trigger, now interpret it
			decoder((char *)usart1_buffer, usart1_level);
			usart1_level = 0;
			clearup_consol();
		}
		else
		{
			//Trigger not met, increment
			usart1_level++;
			//Buffer overflow
			if(usart1_level >= USART1_BUFFER - 1)
			{
				if(usart1_echo == 1)
				{
					print("\n");
				}
				print("[ERROR]: Uart5 buffer is full. Emptieing buffer\n");
				usart1_level = 0;
			}
		}
		usart1_buffer[usart1_level] = '\000';	
	}
}

void check_uart5(void)
{
	while(uart5_calc_rx_level() != 0)
	{
		uart5_buffer[uart5_level] = usart5_get_char();
		if(uart5_echo == 1)
		{
			if((uart5_buffer[uart5_level] == '\010') || (uart5_buffer[uart5_level] == '\127') || (uart5_buffer[uart5_level] == '\177'))
			{
				usart5_put_tx("\010 \010",3);
				if(uart5_level > 0)
				{
					uart5_level--;
					uart5_level--;
				}
			}
			else
			{
				usart5_put_tx((char *)&(uart5_buffer[uart5_level]), 1);
			}
		}
		if(uart5_buffer[uart5_level] == '\r')
		{
			uart5_buffer[uart5_level] = '\000';
			if(uart5_echo)
			{
				print("\n");
			}
			//Recieved trigger, now interpret it
			decoder((char *)uart5_buffer, uart5_level);
			uart5_level = 0;
			clearup_consol();
		}
		else
		{
			//Trigger not met, increment
			uart5_level++;
			//Buffer overflow
			if(uart5_level >= UART5_BUFFER - 1)
			{
				if(uart5_echo == 1)
				{
					print("\n");
				}
				print("[ERROR]: Uart5 buffer is full. Emptieing buffer\n");
				uart5_level = 0;
			}
		}
		uart5_buffer[uart5_level] = '\000';	
	}
}

void clearup_consol(void)
{
	uint32_t i = 0;
	if(uart5_echo == 1)
	{
		while(i < uart5_level)
		{
			usart5_put_tx((char *)&uart5_buffer[i],1);
			i++;
		}
	}
	i = 0;
	if(usart1_echo == 1)
	{
		while(i < usart1_level)
		{
			usart1_put_tx((char *)&usart1_buffer[i],1);
			i++;
		}
	}
}

void consol_puts(char *s)
{
	while( *s != '\000')
	{
		if(uart5_consol == 1)
			usart5_put_tx(s, 1);
		usart1_put_tx(s, 1);
		if(*s == '\n')
		{
			usart1_put_tx("\r", 1);
			if(uart5_consol == 1)
				usart5_put_tx("\r",1);
		}
		s++;
	}
}

void print(char *s)
{
	consol_puts(s);
}

