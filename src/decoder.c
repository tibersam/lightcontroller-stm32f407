

#include "string_helperfunctions.h"
#include "decoder.h"
#include "consol.h"
#include "lightcontrol.h"
#include "atx.h"
#include "timeout_module.h"
#include "button.h"
#include "ws2812b.h"

void setrgb(char *s, int len);

void setmode(char *s, int len);

void process_set(char *s, int len);

long long int get_number(char **s, int *len, int bytenumber);

void process_setlimitrgb(char *s, int len);

void set_stepnumber(char *s, int len);

void set_atx(char *s, int len);

void process_status(void);

void process_echo(char *s, int len);

void process_uart5(char *s, int len);

void process_timeout(char *s, int len);

void process_ligths(char *s, int len);

void process_get(char *s, int len);

void process_get_rgbw(char *s, int len);

void decoder(char *s, int len)
{
	int pos = 0;
	reset_timeout();
	make_lower_case(s, len);
	
	pos = my_strcmp(s, "set", len, 3);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		process_set(s, len);
		return;
	}
	pos = my_strcmp(s, "get", len, 3);
	if(pos != -1)
	{
		process_get(s + pos, len - pos);
		return;
	}
	if(my_strcmp(s, "version", len, 6) != -1)
	{
		print("[OK]: Version 1.0 mit Platine 1.0\n");
		return;
	}
	if(my_strcmp(s, "status", len, 6) != -1)
	{
		process_status();
		return;
	}
	if(my_strcmp(s, "help", len, 4) != -1)
	{
		print("+++++++++++++++++++++++++++++++++++++++++\n");
		print("High level commands:\n");
		print("	set, get, version, status\n");
		print("set commands:\n");
		print("	limit: Set a range of leds the collor\n");
		print("		set limit r g b offset length mode\n");
		print("	mode: set default animation mode\n");
		print("		set mode m\n");
		print("		0: alle gleichzeitig\n");
		print("		1: Lauflicht vom beamer weg\n");
		print("		2: Lauflicht zum beamer\n");
		print("	step: Step delay bei animation. (20ms steps)\n");
		print("		set step s\n");
		print("	rgb: Setze alle leds auf rgb value\n");
		print("		set rgb r g b\n");
		print("	atx: set state of atx\n");
		print("		set atx on|off\n");
		print("	echo: set echo behaviour\n");
		print("		set echo on|off\n");
		print("	uart5con: enable/disable uart 5 consol\n");
		print("		set uart5con on|off\n");
		print(" timeout: set the timeout ammount befor\n");
		print("         the atx is switched off\n");
		print("	        set timeout {number}\n");
		print(" lights: on or off\n");
		print("         set lights on|off\n");
		print("get commands:\n");
		print(" rgbw: returns the rgbw values of leds\n");
		print("      with pos and len\n");
		print("       get rgbw pos len\n");
		print("+++++++++++++++++++++++++++++++++++++++++\n");
		return;
	}
	print("[ERROR]: no Commando found\n");
}

void process_status(void)
{
	print("[OK]: ATX ");
	if(get_atx_status())
	{
		print("on");
	}
	else
	{
		print("off");
	}
	print(" \n");
}

void process_get(char *s, int len)
{
	int pos = my_strcmp(s, "rgbw", len, 4);
	if(pos != -1)
	{
		process_get_rgbw( s + pos, len - pos);
	}
}

void process_get_rgbw(char *s, int len)
{
	long long int numposled = get_number(&s, &len, 2);
	long long int ledlen = get_number(&s, &len, 2);
	if((numposled == -1) || (ledlen == -1))
	{
		print("[ERROR]: numberled or len not correctly decoded\n");
		return;
	}
	if(numposled + ledlen >= NUMBERLED)
	{
		print("[ERROR]: To many led were addressed\n");
	}
	print("[OK]: ");
	for(int i = 0; i < ledlen; i++)
	{
		uint8_t red, green, blue, white;
		getrgbwLED(numposled + i, &red, &green, &blue, &white);
		char_to_asciihex(red);
		print(" ");
		char_to_asciihex(green);
		print(" ");
		char_to_asciihex(blue);
		print(" ");
		char_to_asciihex(white);
		print(" | ");
	}
	print("\n");
}

void process_set(char *s, int len)
{
	int pos = 0;
	pos = my_strcmp(s, "limit", len, 5);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		process_setlimitrgb(s, len);
		return;
	}
	pos = my_strcmp( s, "mode", len, 4);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		setmode(s, len);
		return;
	}
	pos = my_strcmp( s, "step", len, 4);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		set_stepnumber(s, len);
		return;
	}
	pos = my_strcmp( s, "rgb", len, 3);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		setrgb(s, len);
		return;
	}
	pos = my_strcmp( s, "atx", len, 3);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		set_atx( s, len);
		return;
	}
	pos = my_strcmp( s, "echo", len, 4);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		process_echo(s, len);
		return;
	}
	pos = my_strcmp( s, "uart5con", len, 8);
	if(pos != -1)
	{
		s = s + pos;
		len = len - pos;
		process_uart5(s, len);
		return;
	}
	pos = my_strcmp( s, "lights", len, 6);
	if(pos != -1)
	{
		process_ligths(s + pos, len - pos);
		return;
	}
	pos = my_strcmp( s, "timeout", len, 7);
	if(pos !=- 1)
	{
		process_timeout(s + pos, len - pos);
		return;
	}
	print("[ERROR]: No set subcommand found\n");
}

void process_ligths(char *s, int len)
{
	if(my_strcmp(s, "off", len, 3) != -1)
	{
		process_atx_switch(1);
		print("[OK]: lights off\n");
		return;
	}
	if(my_strcmp(s, "on", len, 2) != -1)
	{
		process_atx_switch(2);
		print("[OK]: lights on\n");
		return;
	}
	print("[ERROR]: lights only takes on|off\n");
}

void process_timeout(char *s, int len)
{
	long long int timeout = get_number( &s, &len, 4);
	if(timeout == -1)
	{
		print("[ERROR]: Error in decoding timeout\n");
		return;
	}
	set_timeouttime((uint64_t) timeout);
	print("[OK]: Set timeout\n");
}

void process_uart5(char *s, int len)
{
	if( my_strcmp(s, "off", len, 3) != -1)
	{
		disable_uart5_consol();	
		print("[OK]: Disable uart5 consol\n");
		return;
	}
	if( my_strcmp(s, "on", len, 2) != -1)
	{
		enable_uart5_consol();
		print("[OK]: Enable uart5 consol\n");
		return;
	}
	print("[ERROR]: uart5con on or off\n");
}

void process_echo(char *s, int len)
{
	if( my_strcmp(s, "off", len, 3) != -1)
	{
		set_echo(0);
		print("[OK]: Disable echo\n");
		return;
	}
	if( my_strcmp(s, "on", len, 2) != -1)
	{
		set_echo(1);
		print("[OK]: Enable echo\n");
		return;
	}
	print("[ERROR]: echo on or off\n");
}


void set_atx(char *s, int len)
{
	if( my_strcmp(s, "off", len, 3) != -1)
	{	
		disable_atx();
		return;
	}
	if( my_strcmp(s, "on", len, 2) != -1)
	{
		enable_atx();
		return;
	}
}

void set_stepnumber(char *s, int len)
{
	long long int step = get_number( &s, &len, 4);
	if(step == -1)
	{
		print("[ERROR] Missing step argument\n");
		return;
	}
	set_waitlength((int) step);
	print("[OK]: Set step length\n");
}

long long int get_number(char **s, int *len, int numbytes)
{
	int pos;
	pos = find_next_argument(*s, *len);
	if(pos == -1)
	{
		return -1;
	}
	*s = *s + pos;
	*len = *len - pos;
	return asciinum_to_int_flex(*s, *len, numbytes);
}

void process_setlimitrgb(char *s, int len)
{
	long long int r, g, b, offset, length, mode;
	r = get_number( &s, &len, 1);
	g = get_number( &s, &len, 1);
	b = get_number( &s, &len, 1);
	offset = get_number( &s, &len, 2);
	length = get_number( &s, &len, 2);
	mode = get_number( &s, &len, 1);
	
	if((r == -1) || (g == -1) || (b == -1) || (offset == -1) || (length == -1)|| (mode == -1))
	{
		print("[ERROR]: Somthing went wrong!\n");
		print("[ERROR]: set limit r g b offset length mode\n");
		return;
	}
	setlimitrgb((int) r, (int) g, (int) b, (int) offset, (int) length, (int) mode);
	print("[OK]: set limit rgb done\n");
}

void setmode(char *s, int len)
{
	int pos = find_next_argument(s, len);
	if(pos == -1)
	{
		print("[ERROR]: No value found in setmode\n");
		return;
	}
	if(set_stepmode(asciinum_to_int_flex(s+pos , len - pos, 1)) == -1)
	{
		print("[ERROR]: Stepmode only valid between 0-2\n");
	}
	else
	{
		print("[OK]: Set stepmode\n");
	}
}

void setrgb(char *s, int len)
{
	int posr, posg, posb;
	long long int r, g, b;
	posr = find_next_argument(s, len);
	if(posr == -1)
	{
		print("[ERROR]: Argument to short, Red missing for setrgb\n");
		return;
	}
	posg = find_next_argument( s + posr, len - posr);
	if(posg == -1)
	{
		print("[ERROR]: Argument to short, Green missing for setrgb\n");
		return;
	}
	posg = posg + posr;
	posb = find_next_argument( s + posg, len - posg);
	if(posb == -1)
	{
		print("[ERROR]: Argument to short, Blue missing for setrgb\n");
		return;
	}
	posb = posb + posg;
	r = asciinum_to_int_flex( s + posr, len - posr, 1);
	g = asciinum_to_int_flex( s + posg, len - posg, 1);
	b = asciinum_to_int_flex( s + posb, len - posb, 1);

	if((r == -1) || (g == -1) || (b==-1))
	{
		print("[ERROR]: Value Error\n");
		return;
	}
	setrgbvalues((uint8_t) r, (uint8_t) g, (uint8_t) b);
	print("[OK] set rgb values\n");
}

