/********************************
* 	 Author  : Daniel Maizel	*
*	 Date    : 27/05/2019		*
*	 Reviewer: Sasha Limarev	*
*								*
*********************************/

#include <stdio.h>  /* size_t */
#include <assert.h> /* assert */
#include <stdlib.h> /* malloc */

#include "../include/barr.h"

size_t BARRSetBit(size_t arr, int index, int value)
{
	assert(1 == value || 0 == value);
	
	if(1 == value)
	{
		return(BARRSetOn(arr, index));
	}
	else
	{
		return(BARRSetOff(arr, index));
	}
}

size_t BARRSetOn(size_t arr, int index)
{
	assert(index <= 63 && index >=0);

	return((1lu << index) | arr);
}

size_t BARRSetOff(size_t arr, int index)
{
	assert(index <= 63 && index >=0);

	return((~(1lu << index)) & arr);
}

size_t BARRFlip(size_t arr, int index)
{ 
	assert(index <= 63 && index >=0);

	if(BARRIsOn(arr, index))
	{
		return(BARRSetOff(arr, index));
	}
	else
	{
		return(BARRSetOn(arr, index));
	}
}

size_t BARRMirror(size_t arr)
{
	size_t mirror = 0;
	size_t i = 0;
	
	for(i = 0; i < (sizeof(size_t) * 8); ++i)
	{
		mirror = mirror | ((arr >> i) & 1) << ((sizeof(size_t) * 8 - 1) - i); 
	}
	
	return mirror;
}

static unsigned char *LutInit1()
{
	int i = 0, temp = 0;
	static unsigned char *LUT = NULL;
	if(LUT == NULL)
	{
		LUT = (unsigned char *)malloc(sizeof(unsigned char)*256);
	}
	for(; i < 256; ++i)
	{
		temp = i;
		temp = ((temp & 0xf0) >> 4) | ((temp & 0x0f) << 4);
		temp = ((temp & 0xcc) >> 2) | ((temp & 0x33) << 2);
		temp = ((temp & 0xaa) >> 1) | ((temp & 0x55) << 1);
		
		LUT[i] = temp; 
	}

		return LUT;
}

size_t BARRMirrorLUT(size_t arr)
{
	int i = 0;
	unsigned char current_byte = 0;
	size_t mirror = 0; 
	unsigned char *p_LUT = LutInit1();
	
	for(; i<8; i++)
	{
		current_byte = (arr & 0xFF);
		arr >>= 8;
		current_byte = p_LUT[(unsigned char)current_byte];
		mirror = mirror | (size_t)(current_byte << ((sizeof(size_t)-1-i)*8));
	}

	return mirror;
}

int BARRIsOn(size_t arr, int index)
{
	assert(index <= 63 && index >=0);

	return(1 == ((arr >> index) & 1) );
}

int BARRIsOff(size_t arr, int index)
{
	assert(index <= 63 && index >=0);

	return(0 == ((arr >> index) & 1));
}

size_t BARRRotateLeft(size_t arr, size_t amount)
{
	amount = amount % (sizeof(size_t) * 8);
	
	return(arr << amount | arr>>(sizeof(size_t) * 8 - amount));
}

size_t BARRRotateRight(size_t arr, size_t amount)
{
	amount = amount % (sizeof(size_t) * 8);
	
	return(arr >> amount | arr << (sizeof(size_t) * 8 - amount));
}

size_t BARRCountOn(size_t arr)
{
	int count = 0;
	
	while(1 <= arr) 
	{	
		if((arr & 1) == 1)
		{
			++count;
		}
		arr = arr >> 1;
	}
	
	return count;
}

static unsigned char *LutInit2()
{
	int i = 0, temp = 0, count = 0;
	static unsigned char *LUT2 = NULL;
	
	if(LUT2 == NULL)
	{
		LUT2 = (unsigned char *)malloc(sizeof(unsigned char)*256);
	}
	for(; i < 256; ++i)
	{
		temp = i;
		count = 0;
	
		while(1 <= temp) 
		{	
			if((temp & 1) == 1)
			{
				++count;
			}
			temp = temp >> 1;
		}
		LUT2[i] = count; 
	}
	
	return LUT2;		
}

size_t BARRCountOnLUT(size_t arr)
{
	int i = 0;
	unsigned char current_byte = 0;
	size_t count = 0; 
	unsigned char *p1_LUT = LutInit2();
	
	for(; i<8; i++)
	{
		current_byte = (arr & 0xFF);
		arr >>= 8;
		count += p1_LUT[(unsigned char)current_byte];
	}

	return count;
}

size_t BARRCountOff(size_t arr)
{
	int count = 0;
	
	while(1 <= arr) 
	{	
		if(1 == (arr & 1))
		{
			++count;
		}
		arr = arr >> 1;
	}
	
	return ((sizeof(size_t) * 8) - count);
}
