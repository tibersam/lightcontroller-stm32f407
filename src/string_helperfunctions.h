#ifndef STRING_HELPERFUNCTIONS
#define STRING_HELPERFUNCTIONS

#include <stdint.h>

/*
 * void make_lower_case(char *s, int len)
 *
 * Make Upper case letters to lower case letters inside the string
 */
void make_lower_case(char *s, int len);

/**
 *
 * 	This function is an implementation of a string compare. The difference is,
 * 	that it also possible to tell the length of the string to stop buffer overflow problems.
 */
int my_strcmp(char *s, char *t, int s_len, int t_len);

/**
 * 	Searches for the next space in the string. Arguments need to be seperated by space.
 * 	Return value will point to the next non space character.
 */
int find_next_argument(char *s, int s_length);

/**
 * This function will translate the numbers provided to it to integer. Encoding can be
 * Hex, octal, binary or decimal
 */
long long int asciinum_to_int_flex(char *s, int s_len, int num_bytes);

/**
 * implementation of b^e for decimal operations.
 */
int pot(int base, int exponent);

/**
 * Translate decimal number to integer. max is 32bit integer
 */
long long int translate_dec_format(char *s, int s_len, int max_char);

/**
 * Translate Hex, octal, binary numbers to integer.
 */
long long int translate_number_format(char *s, int s_len, int max_char, int num_bit_coding);

/**
 * returns the normal value to a hex character. If it is not
 * 0-9 or a-f it returns 0xff as an indication it is invalid
 *
 * @param s character to translate
 * @return value of translated char
 */
uint8_t asciihex_to_int(char s);
#endif
