#include "common.h"

#define ERROR_CODE_NO_DOT 1

//------- PRIVATE FUNCTIONS -------------


/*
@NOTE:
   the function assumes at least one digit before the dot

*/
double str_to_float(char* str, uint8_t length)
{
	uint8_t dot_idx = 0;
	int num1 = 0, num2 = 0;
	
	// get the index of the decimal dot
	for(uint8_t i=0; i < length; i++)
	{
		if(str[i] == '.')
		{
			dot_idx = i;
			break;
		}
	}
	
	// check if we found the dot
	if(dot_idx == 0) return ERROR_CODE_NO_DOT;
	
	// calculate the float
	int mul = 1;
	for(int16_t i=dot_idx-1; i >= 0; i--)
	{
		num1 += (str[i] - '0') * mul;
		mul *= 10;
	}

	mul = 1;
	for(int16_t i=length-1; i > dot_idx; i--)
	{
		num2 += (str[i] - '0') * mul;
		mul *= 10;
	}
	
	return (double)num1 + (double)num2 / (double)mul;
}

/*
Used when the string length is not known

@param get_len: a pointer to where we return the string length
*/
double str_to_float_nl(char* str, uint8_t* get_len)
{
	// determine the length of the string
	uint8_t len = 0;
	
	uint8_t idx = 0;
	
	while( is_digit(str[idx]) || str[idx] == '.')
	{
		len++;
		idx++;
	}
	
	// pass the actual length to parameter pointer
	if(get_len != 0) *get_len = len;
	
	// get the float value
	return str_to_float(str, len);
	
}

inline uint8_t is_digit(char c)
{
	return (c >= '0' && c <= '9');
}