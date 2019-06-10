#include <stdlib.h> /* malloc */
#include <assert.h> /* assert */

#include "../../include/DLL/dll.h" /* it_t */
#include "../../include/SRTL/srtl.h" /* it_t */

struct srtl
{
	dll_t *list;
	cmp_f is_before;
	void *params;
};

srtl_t *SrtLCreate(cmp_f is_before, const void *params)
{
	srtl_t *srtl = (srtl_t *)malloc(sizeof(srtl_t));
	if(NULL == srtl)
	{
		free(srtl);
		return NULL;
	}
	
	srtl->list = DLLCreate();
	srtl->is_before = is_before;
	srtl->params = (void *)params;
	
	return srtl;
}

void SrtLDestroy(srtl_t *srtl)
{
	assert(NULL != srtl);
	
	DLLDestroy(srtl->list);
	free(srtl);
}

size_t SrtLSize(const srtl_t *srtl)
{
	assert(NULL != srtl);
	
	return DLLSize(srtl->list);
}

int SrtLIsEmpty(const srtl_t *srtl)
{
	assert(NULL != srtl);

	return DLLIsEmpty(srtl->list);
}

sit_t SrtLInsert(srtl_t *srtl, const void *data)
{
	sit_t where = SrtLFind(DLLBegin(srtl->list), 
	DLLEnd(srtl->list), srtl->is_before, srtl->params, (void *)data);

	return DLLInsert(srtl->list, where, (void *)data);
}


/*sit_t SrtLErase(sit_t where)
{
	return(DLLErase);
}

void SrtLPopFront(srtl_t *srtl)
{
	assert(NULL != srtl);

	DLLErase(DLLBegin(srtl->list));
}

void SrtLPopBack(srtl_t *srtl)
{
	assert(NULL != srtl);

	DLLErase(DLLPrev(DLLEnd(srtl->list)));
}*/

sit_t SrtLFind(sit_t from, sit_t to, cmp_f compare, const void *params, const void *key)
{
	return DLLFind(from, to,compare, (void *)params, (void *)key);
}

sit_t SrtLBegin(const srtl_t *srtl)
{
	return DLLBegin(srtl->list);
}

sit_t SrtLEnd(const srtl_t *srtl)
{
	return DLLEnd(srtl->list);
}

