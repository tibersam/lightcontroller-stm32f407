#include "button_consol.h"
#include "consol.h"
#include "string_helperfunctions.h"
#include "wait.h"
#include "lightcontrol.h"

#define RED 0
#define GREEN 1
#define BLUE 2
#define YELLOW 3

uint64_t tr;
uint8_t r;
uint64_t tg;
uint8_t g;
uint64_t tb;
uint8_t b;
uint64_t ty;
uint8_t y;



uint8_t detect_collour(char *s, int len);

void button_decode_update(char *s, int len);

void button_decode_time(char *s, int len);


void button_test(void)
{
	wait(500);
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

void button_decoder_init(void)
{
	tr = 0;
	tg = 0;
	tb = 0;
	ty = 0;
	r = 0;
	g = 0;
	b = 0;
	y = 0;
}

void button_decoder(char *s, int len)
{
	int pos = 0;
	make_lower_case(s, len);
	//print(s);
	//print("\n");
	pos = my_strcmp(s, "[time]", len, 6);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		pos = find_next_argument(s, len);
		if(pos == -1)
		{
			return;
		}
		s = s + pos;
		len = len - pos;
		button_decode_time(s, len);
	}
	pos = my_strcmp(s, "[update]", len, 8);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		pos = find_next_argument(s, len);
		if(pos == -1)
		{
			return;
		}
		s = s + pos;
		len = len - pos;
		button_decode_update(s, len);
	}
}

uint8_t detect_collour(char *s, int len)
{
	if( my_strcmp(s, "red", len, 3) != -1)
		return RED;
	if( my_strcmp(s, "green", len, 5) != -1)
		return GREEN;
	if( my_strcmp(s, "blue", len, 4) != -1)
		return BLUE;
	if( my_strcmp(s, "yellow", len, 6) != -1)
		return YELLOW;
	return 255;
}

void button_decode_update(char *s, int len)
{
	int pos = 0;
	long long int state = 0;
	pos = find_next_argument(s, len);
	if(pos == -1)
	{
		print("[ERROR]:somthing went wrong: ");
		print(s);
		print("\n");
		return;
	}
	state = asciinum_to_int_flex(s + pos, len - pos, 1);
	if(state ==  -1)
	{
		print("[ERROR]: State could not be decoded \n");
		return;
	}
	switch(detect_collour(s, len))
	{
	case RED:
		r = (uint8_t) state;
		tr = get_tick();
		break;
	case GREEN:
		g = (uint8_t) state;
		tg = get_tick();
		break;
	case BLUE:
		b = (uint8_t) state;
		tb = get_tick();
		break;
	case YELLOW:
		y = (uint8_t) state;
		ty = get_tick();
		break;
	default:
		print("[ERROR]: Could not detect collour\n");
		return;
		break;
	}

}

void button_decode_time(char *s, int len)
{
	int pos = 0;
	long long int time = 0;
	pos = find_next_argument(s, len);
	if(pos == -1)
	{
		print("[ERROR]:somthing went wrong: ");
		print(s);
		print("\n");
		return;
	}
	time = asciinum_to_int_flex(s + pos, len - pos, 4);
	if(time == (long long int) -1)
	{
		print("[ERROR]: somthing went wrong\n");
		return;
	}
	if(time < 500)
	{
		/*switch(detect_collour(s, len))
		{
		case RED:
			setrgbvalues(255, 0, 0);
			break;
		case GREEN:
			setrgbvalues(0, 255, 0);
			break;
		case BLUE:
			setrgbvalues( 0, 0, 255);
			break;
		case YELLOW:
			setrgbvalues(255, 255, 255);
			break;
		case 255:
		default:
			print("[ERROR]: collour not detected: ");
			print(s);
			print("\n");
			return;
			break;
		}*/
		uint8_t tmpr = 0;
		uint8_t tmpb = 0;
		uint8_t tmpg = 0;
		uint64_t tick = get_tick();
		if(tick - tr < 500 && r == 0)
			tmpr = 255;
		if(tick - tg < 500 && g == 0)
			tmpg = 255;
		if(tick - tb < 500 && b == 0)
			tmpb = 255;
		if(tick - ty < 500 && y == 0)
		{
			if(tmpr == 0)
				tmpr = 100;
			if(tmpg == 0)
				tmpg = 100;
			if(tmpb == 0)
				tmpb = 100;
			if((tmpr == 100) && (tmpg == 100) && (tmpb == 100))
			{
				tmpr = 255;
				tmpg = 255;
				tmpb = 255;
			}
		}
		setrgbvalues(tmpr, tmpg, tmpb);
	}

}
