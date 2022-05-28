/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    findfp.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2012/11/28
 *
 * Description:
 *    暂只支持静态分配文件指针, 静态空间不足后动态分配功能后续开发
 **************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "local.h"
#include "glue.h"

#define FOPEN_MAX 20


/* the usual - (stdin + stdout + stderr) */
static FILE usual[FOPEN_MAX - 3];
static struct glue uglue = { 0, FOPEN_MAX - 3, usual };

struct glue __sglue = { &uglue, 3, __sF };

/*
 * Find a free FILE for fopen et al.
 */
 
/*+\NEW\zhuwangbin\2020.3.29\ lua解析没有使用自带的__sfp接口，使用了8910 newlib中的接口导致内存泄漏*/
FILE *
LUA__sfp(void)
{
	FILE *fp;
	int n;
	struct glue *g;

	for (g = &__sglue;; g = g->next) {
		for (fp = g->iobs, n = g->niobs; --n >= 0; fp++)
			if (fp->_flags == 0)
				goto found;
		if (g->next == NULL)
			break;
	}
	return (NULL);
    
found:
	fp->_flags = 1;		/* reserve this slot; caller sets real flags */

	return (fp);
}
/*-\NEW\zhuwangbin\2020.3.29\ lua解析没有使用自带的__sfp接口，使用了8910 newlib中的接口导致内存泄漏*/

