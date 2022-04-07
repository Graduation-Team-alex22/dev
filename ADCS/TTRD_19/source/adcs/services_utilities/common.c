#include "common.h"
#include "math.h"

#define ERROR_CODE_NO_DOT 1

//------- PRIVATE FUNCTIONS -------------

//------- PRIVATE STRUCTURES -------------

union _cnv {
    double cnvD;
    float cnvF;
    uint32_t cnv32;
    uint16_t cnv16[4];
    uint8_t cnv8[8];
};

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

inline __attribute((always_inline)) double vect_magnitude(xyz_t vect)
{
     return sqrt( vect.x*vect.x + vect.y*vect.y + vect.z*vect.z );
}

inline __attribute((always_inline)) double vect_magnitude_arr(double vect[])
{
   return sqrt( vect[0]*vect[0] + vect[1]*vect[1] + vect[2]*vect[2] );
}

// need to check endiannes
void cnv32_8(const uint32_t from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnv32 = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];
    to[2] = cnv.cnv8[2];
    to[3] = cnv.cnv8[3];
}

void cnv16_8(const uint16_t from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnv16[0] = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];

}

void cnv8_32(uint8_t *from, uint32_t *to) {

    union _cnv cnv;

    cnv.cnv8[3] = from[3];
    cnv.cnv8[2] = from[2];
    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnv32;

}

void cnv8_16LE(uint8_t *from, uint16_t *to) {

    union _cnv cnv;

    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnv16[0];
}

void cnv8_16(uint8_t *from, uint16_t *to) {

    union _cnv cnv;

    cnv.cnv8[1] = from[0];
    cnv.cnv8[0] = from[1];
    *to = cnv.cnv16[0];
}


void cnvF_8(const float from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnvF = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];
    to[2] = cnv.cnv8[2];
    to[3] = cnv.cnv8[3];
}

void cnv8_F(uint8_t *from, float *to) {

    union _cnv cnv;

    cnv.cnv8[3] = from[3];
    cnv.cnv8[2] = from[2];
    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnvF;

}

void cnvD_8(const double from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnvD = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];
    to[2] = cnv.cnv8[2];
    to[3] = cnv.cnv8[3];
    to[4] = cnv.cnv8[4];
    to[5] = cnv.cnv8[5];
    to[6] = cnv.cnv8[6];
    to[7] = cnv.cnv8[7];
}

void cnv8_D(uint8_t *from, double *to) {

    union _cnv cnv;

    cnv.cnv8[7] = from[7];
    cnv.cnv8[6] = from[6];
    cnv.cnv8[5] = from[5];
    cnv.cnv8[4] = from[4];
    cnv.cnv8[3] = from[3];
    cnv.cnv8[2] = from[2];
    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnvD;

}

double norm(double x1, double x2, double x3) {
    return sqrt(x1 * x1 + x2 * x2 + x3 * x3);
}
