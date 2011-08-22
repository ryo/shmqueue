/*	$Id: rtailq.h,v 1.7 2011/08/21 11:50:33 ryo Exp $	*/

/*-
 * Copyright (c) 2011 SHIMIZU Ryo <ryo@nerv.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTAILQ_H_
#define _RTAILQ_H_

/*
 * relocatable TAILQ
 */

#include <stdint.h>

#define RTAILQ_HEAD(name)						\
struct name {								\
	uintptr_t rtqh_first;						\
	uintptr_t rtqh_last;						\
}

#define RTAILQ_HEAD_INITIALIZER(head)					\
	{ 0, 0 }

#define RTAILQ_ENTRY(type)						\
struct {								\
	uintptr_t rtqe_next;						\
	uintptr_t rtqe_prev;						\
}

/*
 * RTAILQ Functions
 */
#define RTAILQ_PTR2OFF(ptr, head)					\
	((uintptr_t)(ptr) - (uintptr_t)(head))

#define _RTAILQ_OFF2PTR(off, head, type)				\
	((type *)(((uintptr_t)(off)) + ((uintptr_t)(head))))

#define RTAILQ_OFF2PTR(off, head, type)					\
	_RTAILQ_OFF2PTR(off, head, struct type)				\

#define RTAILQ_INIT(head) do {						\
	(head)->rtqh_first = 0;						\
	(head)->rtqh_last = 0;						\
} while (/*CONSTCOND*/0)

#define RTAILQ_INSERT_HEAD(head, elm, type, field) do {			\
	if (((elm)->field.rtqe_next = (head)->rtqh_first) != 0)		\
		RTAILQ_OFF2PTR((head)->rtqh_first, head,		\
		    type)->field.rtqe_prev =				\
		    RTAILQ_PTR2OFF(&(elm)->field.rtqe_next, head);	\
	else								\
		(head)->rtqh_last =					\
		    RTAILQ_PTR2OFF(&(elm)->field.rtqe_next, head);	\
	(head)->rtqh_first = RTAILQ_PTR2OFF(elm, head);			\
	(elm)->field.rtqe_prev =					\
	    RTAILQ_PTR2OFF(&(head)->rtqh_first, head);			\
} while (/*CONSTCOND*/0)

#define RTAILQ_INSERT_TAIL(head, elm, type, field) do {			\
	(elm)->field.rtqe_next = 0;					\
	(elm)->field.rtqe_prev = (head)->rtqh_last;			\
	*_RTAILQ_OFF2PTR((head)->rtqh_last, head, uintptr_t) =		\
	    RTAILQ_PTR2OFF(elm, head);					\
	(head)->rtqh_last = RTAILQ_PTR2OFF(&(elm)->field.rtqe_next,	\
	    head);							\
} while (/*CONSTCOND*/0)

#define RTAILQ_REMOVE(head, elm, type, field) do {			\
	if ((elm)->field.rtqe_next != 0)				\
		RTAILQ_OFF2PTR((elm)->field.rtqe_next, head,		\
		    type)->field.rtqe_prev = (elm)->field.rtqe_prev;	\
	else								\
		(head)->rtqh_last = (elm)->field.rtqe_prev;		\
	*_RTAILQ_OFF2PTR((elm)->field.rtqe_prev, head, uintptr_t) =	\
	    (elm)->field.rtqe_next;					\
} while (/*CONSTCOND*/0)

#define RTAILQ_FOREACH(var, head, type, field)				\
	for ((var) = RTAILQ_OFF2PTR((head)->rtqh_first, head, type);	\
	    RTAILQ_PTR2OFF(var, head) != 0;				\
	    (var) = RTAILQ_OFF2PTR((var)->field.rtqe_next, head, type))

#define RTAILQ_EMPTY(head)						\
	((head)->rtqh_first == 0)

#define RTAILQ_FIRST(head, type)					\
	(RTAILQ_EMPTY(head) ? NULL :					\
	RTAILQ_OFF2PTR((head)->rtqh_first, head, type))

#define RTAILQ_NEXT(elm, head, type, field)				\
	(((elm)->field.rtqe_next == 0) ? NULL :				\
	RTAILQ_OFF2PTR((elm)->field.rtqe_next, head, type))

#define RTAILQ_LAST(head, type)						\
	_RTAILQ_OFF2PTR(_RTAILQ_OFF2PTR(				\
	    (head)->rtqh_last, head, uintptr_t *)[1], head,		\
	    uintptr_t)[0] == 0 ? NULL :					\
	    RTAILQ_OFF2PTR(_RTAILQ_OFF2PTR(_RTAILQ_OFF2PTR(		\
	    (head)->rtqh_last, head, uintptr_t *)[1], head,		\
	    uintptr_t)[0], head, type)

#define RTAILQ_PREV(elm, head, type, field)				\
	(((*_RTAILQ_OFF2PTR(_RTAILQ_OFF2PTR(				\
	    (elm)->field.rtqe_prev, head, uintptr_t *)[1], head,	\
	    uintptr_t)) == 0) ? NULL :					\
	    RTAILQ_OFF2PTR(*_RTAILQ_OFF2PTR(_RTAILQ_OFF2PTR(		\
	    (elm)->field.rtqe_prev, head, uintptr_t *)[1], head,	\
	    uintptr_t), head, type))


/*
 * for debug
 */
#define RTAILQ_DUMP_HEAD(head, type, prefix) do {			\
	printf(prefix "head = %p\n", head);				\
	printf(prefix "head->rtqh_first = VAL:%p (PTR:%p)\n",		\
	    (void*)((head)->rtqh_first),				\
	    RTAILQ_OFF2PTR((head)->rtqh_first, head, type));		\
	printf(prefix "head->rtqh_last = VAL:%p (PTR:%p)\n",		\
	    (void*)((head)->rtqh_last),					\
	    RTAILQ_OFF2PTR((head)->rtqh_last, head, type));		\
} while (/*CONSTCOND*/0)

#define RTAILQ_DUMP(elm, head, type, field, prefix) do {		\
	printf(prefix "elm = %p\n", elm);				\
	printf(prefix "elm->rtqe_next = VAL:%p (PTR:%p)\n",		\
	    (void*)((elm)->field.rtqe_next),				\
	    RTAILQ_OFF2PTR((elm)->field.rtqe_next, head, type));	\
	printf(prefix "elm->rtqe_prev = VAL:%p (PTR:%p)\n",		\
	    (void*)((elm)->field.rtqe_prev),				\
	    RTAILQ_OFF2PTR((elm)->field.rtqe_prev, head, type));	\
} while (/*CONSTCOND*/0)

#endif /* _RTAILQ_H_ */
