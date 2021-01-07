
#include "string_helperfunctions.h"
#include "consol.h"

/*
 * void make_lower_case(char *s, int len)
 *
 * Make Upper case letters to lower case letters inside the string
 */
void make_lower_case(char *s, int len)
{
	int i = 0;
	for (i = 0; i < len; i++)
	{
		if(*(s+i) >= 'A' && *(s+i) <= 'Z')
		{
			*(s+i) = *(s+i) + 32;
		}
	}
}



/**
 *
 * 	This function is an implementation of a string compare. The difference is,
 * 	that it also possible to tell the length of the string to stop buffer overflow problems.
 */
int my_strcmp(char *s, char *t, int s_len, int t_len)
{
	int max_offset = s_len -t_len;
	int i = 0, j = 0;

	if (max_offset < 0)
		return -1;
	while( i <= max_offset)
	{
		j = 0;
		while(*(s+i+j) == *(t+j))
		{
			j++;
			if ( i + j > s_len)
				return -1;
			if(j == t_len)
				return i;
		}
		i++;
	}
	return -1;
}



/**
 * 	Searches for the next space in the string. Arguments need to be seperated by space.
 * 	Return value will point to the next non space character.
 */
int find_next_argument(char *s, int s_length)
{
	char *tmp;
	tmp = s;
	while(*tmp != ' ')
	{
		tmp++;
		if((*tmp == 0) || (s_length < tmp -s  ))
		{
			return -1;
		}
	}
	while((*tmp == ' '))
	{
		tmp++;
		if((*tmp == 0 ) || (s_length < tmp - s))
		{
			return -1;
		}
	}
	return tmp - s;
}


/**
 * This function will translate the numbers provided to it to integer. Encoding can be
 * Hex, octal, binary or decimal
 */
long long int asciinum_to_int_flex(char *s, int s_len, int num_bytes)
{
	int len = s_len;
	long long int ret = 0;
	// find start of number
	while(*s == ' ')
		{
			s++;
			len--;
			if(len <= 0 )
				return -1;
		}
	ret = find_next_argument(s, len);
	if (ret != -1)
		len = ret;
	// is it hex code?
	if( my_strcmp(s, "0x", len, 2)==0)
	{
		if (len-2 < 1 )
			return -1;
		return translate_number_format(s+2, len-2, num_bytes * 2, 4);
	}
	//binary?
	if ( my_strcmp(s, "0b", len, 2 ) == 0)
	{
		if (len-2 < 1 )
			return -1;
		return translate_number_format(s+2,len-2,num_bytes*8,1);
	}
	//octal?
	if( my_strcmp(s, "0", len, 1) == 0)
	{
		if (len-1 > 0 )
		{
		ret = translate_number_format(s+1, len-1, num_bytes * 3, 3);
		if(ret != -1)
			{
			print("[WARNING]: Number is treated as octal code!\r\n");
			return ret;
			}
		}
	}
	//seems like it is dec
	ret = translate_dec_format(s, len, 10);
	return ret;
}

/**
 * implementation of b^e for decimal operations.
 */
int pot(int base, int exponent)
{
	int ret = 1;
	for (int i = 0; i < exponent; i++)
	{
			ret = ret*base;
	}
	return ret;
}


/**
 * Translate decimal number to integer. max is 32bit integer
 */
long long int translate_dec_format(char *s, int s_len, int max_char)
{
	char *tmp = s;
	int len = s_len;
	long long int ret = 0;
	long long int max_value = 1ll << 32;
	int i = 0;
	char z;
	while(((*tmp >= '0')&&(*tmp <='9'))||((*tmp >='a') && (*tmp <= 'z')))
	{
		tmp++;
		len--;
		if(len < 0)
			return -1;
	}
	if(tmp==s)
		return -1;
	tmp--;
	while(((tmp-i)!= (s-1)) && (ret < max_value) && (i < max_char))
	{
		z = asciihex_to_int(*(tmp-i));
		if ((z == 0xff) || (z > 9))
		{
			print("[ERROR]: Invalid character for decimal number");
			return -1;
		}
		ret = ret + ((long long int) z) * pot(10,i);
		i++;
	}
	return ret;
}



/**
 * Translate Hex, octal, binary numbers to integer.
 */
long long int translate_number_format(char *s, int s_len, int max_char, int num_bit_coding)
{
	char *tmp = s;
	int len = s_len;
	long long int ret = 0;
	int max_value = 1ll << num_bit_coding;
	char z;
	while(((*tmp >= '0')&&(*tmp <='9'))||((*tmp>= 'a')&&(*tmp <='z')))
	{
		tmp++;
		len--;
		if(len < 0)
			return -1;
	}
	if(tmp==s)
		return -1;
	tmp--;
	for (int i = 0;(tmp-i)!= (s-1);i++ )
	{
		if ((i >= max_char) || (i >= 32/num_bit_coding))
		{
			print("[WARNING]: your argument was to long. MSB part is truncated\r\n");
			break;
		}
		z = asciihex_to_int(*(tmp-i));
		if ((z==0xff) || (z >= max_value) )
		{
			print("[ERROR]: Please provide a valid encoded number");
			return -1;
		}
		ret = ret + (((long long int) z) << (num_bit_coding * i));
	}
	return ret;
}


/**
 * returns the normal value to a hex character. If it is not
 * 0-9 or a-f it returns 0xff as an indication it is invalid
 *
 * @param s character to translate
 * @return value of translated char
 */
uint8_t asciihex_to_int(char s)
{
	uint8_t value = 0;
	switch(s)
	{
		case '0':
			value = 0;
			break;
		case '1':
			value = 1;
			break;
		case '2':
			value = 2;
			break;
		case '3':
			value = 3;
			break;
		case '4':
			value = 4;
			break;
		case '5':
			value = 5;
			break;
		case '6':
			value = 6;
			break;
		case '7':
			value = 7;
			break;
		case '8':
			value = 8;
			break;
		case '9':
			value = 9;
			break;
		case 'a':
			value = 10;
			break;
		case 'b':
			value = 11;
			break;
		case 'c':
			value = 12;
			break;
		case 'd':
			value = 13;
			break;
		case 'e':
			value = 14;
			break;
		case 'f':
			value = 15;
			break;
		default:
			value = 0xff;
			break;
	}
	return value;
}
