#include "button_consol.h"
#include "consol.h"
#include "string_helperfunctions.h"
#include "wait.h"

void button_test(void)
{
	button_box_puts("set red 1\n");
	wait(10);
	check_uart();
	button_box_puts("set green 1\n");
	wait(10);
	check_uart();	
	button_box_puts("set blue 1\n");
	wait(10);
	check_uart();	
	button_box_puts("set yellow 1\n");
	wait(10);
	check_uart();	
	button_box_puts("set white 1\n");
	wait(10);
	check_uart();	
	wait(5000);
	button_box_puts("set red 0\n");
	wait(10);
	check_uart();	
	button_box_puts("set green 0\n");
	wait(10);
	check_uart();	
	button_box_puts("set blue 0\n");
	wait(10);
	check_uart();	
	button_box_puts("set yellow 0\n");
	wait(10);
	check_uart();	
	button_box_puts("set white 0\n");
	wait(10);
	check_uart();	
}


void button_decoder(char *s, int len)
{
	make_lower_case(s, len);
	print(s);
	print("\n");
}
