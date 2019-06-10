/********************************
* 	 Author  : Daniel Maizel	*
*	 Date    : 27/05/2019		*
*	 Reviewer: Alex         	*
*								*
*********************************/

#include <stdlib.h> /* malloc */
#include <assert.h> /* assert */

#include "../../include/DLL/dll.h" /* My header file */
#include "../../include/SRTL/srtl.h" /* My header file */

#define UNUSED(X) (void) (X)

struct DLLnode               
{
	void *data;
	struct DLLnode *next;
	struct DLLnode *prev;
};

struct DLL
{
    struct DLLnode *head;
    struct DLLnode *tail;
};

static it_t DLLCreateNode(const void *data, it_t next, it_t prev)
{
	it_t node = (it_t)malloc(sizeof(struct DLLnode));
	
	if(NULL == node)
	{
		free(node);
		return NULL;
	}
	
	node->data = (void *)data;
	node->next = next;
	node->prev = prev;
	
	return node;
}

dll_t *DLLCreate()
{
	dll_t *dll = (dll_t *)malloc(sizeof(dll_t));
	
	if(NULL == dll)
	{
		free(dll);
		return NULL;
	}
	
	dll->head = DLLCreateNode((void *)0xDEADBEEF, NULL, NULL);
	dll->tail = DLLCreateNode((void *)0xDEADBEEF, NULL, dll->head);
	dll->head->next = dll->tail;
	
	return dll;
}

void DLLDestroy(dll_t *dll)
{
	it_t temp = NULL;
	it_t current = NULL;
	
	assert(NULL != dll);
	current = dll->head;  
	
	while(NULL != current)
	{
		temp = current->next;
		free(current);
		current = temp;
	}
	
	free(dll);
}

size_t DLLSize(const dll_t *dll)
{
	it_t current = dll->head;
	size_t count = 0;
	assert(NULL != dll);
	
	while(current->next != dll->tail)
	{
		current = current->next;
		++count;
	}
	
	return count;
}

it_t DLLPushFront(dll_t *dll, const void *data)
{
	it_t begin = DLLBegin(dll);
	it_t new_node = DLLCreateNode(data ,dll->head->next,dll->head);
	assert(NULL != dll);
	
	dll->head->next = new_node;
	begin->prev = new_node;
	
	return new_node;
}

it_t DLLPushBack(dll_t *dll, const void *data)
{
	it_t new_node = NULL;
	it_t temp = NULL;
	assert(NULL != dll);

	new_node = DLLCreateNode(data ,dll->tail,dll->tail->prev);
	temp = dll->tail->prev;
	dll->tail->prev =new_node;
	temp->next = new_node;
	
	return new_node;
}

it_t DLLBegin(const dll_t *dll)
{
	assert(NULL != dll);
	
	return dll->head->next;
}

it_t DLLEnd(const dll_t *dll)
{
	assert(NULL != dll);
	
	return dll->tail;
}

it_t DLLNext(it_t iter)
{
	return iter->next;
}

it_t DLLPrev(it_t iter)
{
	return iter->prev;
}

void *DLLGetData(const it_t iter)
{
	return iter->data;
}

void DLLPopBack(dll_t *dll)
{
	assert(NULL != dll);

	DLLErase(DLLPrev(DLLEnd(dll)));
}

void DLLPopFront(dll_t *dll)
{
	assert(NULL != dll);

	DLLErase(DLLBegin(dll));
}

it_t DLLErase(it_t where) 
{
	it_t prev = DLLPrev(where);
	it_t next = DLLNext(where);
	prev->next = next;
	next->prev = prev;
	free(where);
   	
   	return next;
}

int DLLIsEmpty(const dll_t *dll)
{
	assert(NULL != dll);
	
	return (DLLBegin(dll) == DLLEnd(dll));
}

it_t DLLInsert(dll_t *dll, it_t iter, const void *data)
{
	it_t new_node = NULL;
	assert(NULL != dll);
	
	new_node = DLLCreateNode(data,iter->next,iter);
	iter->next = new_node;
	new_node->prev = iter;
	
	return new_node;
}

int IsSameIter(const it_t iter1,const it_t iter2)
{
	return(iter1 == iter2);
}

it_t DLLFind(it_t from, it_t to, cmp_f compare, void* params, void* data)
{
	it_t current = NULL;
	int res = 1;
	UNUSED(params);

	for(current = from; current != to ; current = current->next)
	{
		res = (compare)(current->data, data);
		if(res != 0)
		{
			return current;
		}
	}
	
	return to;
}

int DLLForEach(it_t from, it_t to, act_f action, void* params)
{
	it_t current = NULL;
	int error = 0;
	
	for(current = from; current != to ; current = current->next)
	{
		error = (action)(current->data, params);
		if(0 != error)
		{
			return error;
		} 
	}
	
	return 0;
}

void DLLSpliceBefore(it_t where, it_t from, it_t to)
{
	it_t before_where = where->prev;
	it_t before_to = to->prev;
	it_t before_from = from->prev;
	
	from->prev = where->prev;
	before_where->next = from;
	where->prev = before_to;
	before_to->next = where;
	before_from->next = to;
}
