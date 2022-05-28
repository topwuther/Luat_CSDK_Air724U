/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    mallo.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2012/9/15
 *
 * Description:
 * 
 **************************************************************************/

#include "stddef.h"
#include "string.h"
#include "malloc.h"
#include "reent.h"
// extern void *_calloc_r(size_t, size_t);
// extern void  _free_r(void *);
// extern void *_malloc_r(size_t);
// extern void *_realloc_r(void *, size_t);

void* 
lualibc_malloc(size_t bytes) 
{
    return _malloc_r(bytes);
}

void 
lualibc_free(void* mem) 
{
    _free_r(mem);
}

void* 
lualibc_calloc(size_t n_elements, size_t elem_size) 
{    
    return _calloc_r (n_elements, elem_size);
}

void*
lualibc_realloc(void* oldMem, size_t bytes) 
{
    return _realloc_r (oldMem, bytes);
}

