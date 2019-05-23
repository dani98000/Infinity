#include <stdio.h> /* printf */
#include <stdlib.h>/* malloc */

void *memsetV2	(void *str, int c, size_t n);
int main()
{
	void *str = (void *)malloc(28*sizeof(size_t));
	int i = 0;
	memsetV2((char*)str + 1, 5, 25);
	for (;i<30;i++)
	{
		printf("%p, %d\n",(char *)str + i,*((char *)str + i));
	}
	return 0;
}

void *memsetV2(void *str, int c, size_t n)
{
	int i = 0, j = 0, temp = 0;
	char* start = (char *)str;
	size_t *mid = NULL;
	size_t block = 0;
	char *end = NULL;
	
	while( ((size_t)(start) % 8 != 0 ) && (n > 0))
	{
		*(start) = c;
		++start;
		/*printf("c was added to this address: %p\n",(char*)str+i);*/
		--n;
	}
	mid = (size_t *)(start);
	temp = n - n % 8;

	for(j = 0; j < 8; ++j)
	{
		block ^= c;
		block <<= 8;
	}
	block ^= c;
	i = 0;
	while(i < temp/8)
	{ 
		*mid = block;
		++mid;
		++i;
		/*printf("c2 was added to this address: %p\n",&p[i]);*/
		n -= sizeof(size_t);
    }
    end = (char*)(mid);
	while(n > 0)
    {
		*end = c;
		/*printf("c3 was added to this address: %p\n",end+i);*/
		++end;
		--n;
	}
	return 0;
}		  
