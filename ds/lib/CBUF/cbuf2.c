#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>  /* perror */

#include "../../include/CBUF/cbuf.h"

struct CBUF
{
    size_t capacity;
    size_t size;
    size_t read_offset;
    void *buff;
};

cbuf_t *CBUFCreate(size_t nbytes)
{
    cbuf_t *cbuf = malloc(sizeof(cbuf_t));
    assert(nbytes>0);
	if(NULL == cbuf)
	{
		return NULL;
	}
	cbuf->buff = (void *)calloc(nbytes,sizeof(char));
	if (NULL == cbuf->buff)
	{
		return NULL;
	}
	cbuf->capacity = nbytes;
	cbuf->size = 0;
	cbuf->read_offset = 0;
    return cbuf;
}

void CBUFDestroy(cbuf_t *cbuf)
{
	free(cbuf->buff);
	free(cbuf);
}

size_t CBUFRead(cbuf_t *cbuf, void *data, size_t nbytes)
{
	size_t count = 0;

	if(CBUFIsEmpty(cbuf))
	{
		errno = ENODATA;
	}
    while (cbuf->size > 0  && nbytes > 0)
	{
  		((char *)data)[count] = ((char*)cbuf->buff)[cbuf->read_offset];
   		 ++cbuf->read_offset;
   		 --cbuf->size;
   		 ++count;
   		 --nbytes;
    	if (cbuf->read_offset == cbuf->capacity)
		{
	        cbuf->read_offset = 0;
		}
	}
    return count;
}

size_t CBUFWrite(cbuf_t *cbuf, const void *data, size_t nbytes)
{
	size_t count = 0;
	
	while(CBUFFreeSpace(cbuf) > 0 && nbytes > 0)
	{
    	((char*)cbuf->buff)[cbuf->size+cbuf->read_offset] = ((char *)data)[count];
    	++cbuf->size;
    	--nbytes;
    	++count;
    	if (cbuf->size == cbuf->capacity && (cbuf->size+1) % cbuf->capacity < cbuf->read_offset)
    	{
       		cbuf->size = 0;
		}
	}
	if(CBUFFreeSpace(cbuf) == 0)
    {
		errno = EOVERFLOW;
    }
    return count;
}

int CBUFIsEmpty(const cbuf_t *cbuf)
{
	return(cbuf->size == 0);
}

size_t CBUFFreeSpace(cbuf_t *cbuf)
{
	return(cbuf->capacity - cbuf->size);
}

size_t CBUFCapacity(cbuf_t *cbuf)
{
	return (cbuf->capacity);
}
