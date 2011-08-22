/*	$NetBSD: lock_stubs.S,v 1.25 2011/01/12 23:12:12 joerg Exp $	*/

/*-
 * Copyright (c) 2006, 2007, 2008, 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Andrew Doran.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __linux__
#define ENTRY(name)	\
.globl name;	\
.align 4,0x90;	\
name:
#endif

#ifdef __NetBSD__
#include <machine/asm.h>
#endif
.text


/*
 * Spinlocks.
 */
ENTRY(__cpu_simple_lock_init)
#if defined(__amd64)
	movb	$0, (%rdi)
	ret

#elif defined(__i386)
	movl	4(%esp), %edx
	movb	$0, (%edx)
	ret

#else
#error "unknown cpu"
#endif


ENTRY(__cpu_simple_lock)
#if defined(__amd64)
	movl	$0x0100, %eax
1:
	lock
	cmpxchgb %ah, (%rdi)
	jnz	2f
	ret
	nop
	nop
	ret
2:
	movl	$0x0100, %eax
	pause
	nop
	nop
	cmpb	$0, (%rdi)
	je	1b
	jmp	2b

#elif defined(__i386)
	movl	4(%esp), %edx
	movl	$0x0100, %eax
1:
	lock
	cmpxchgb %ah, (%edx)
	jnz	2f
	ret
	nop
	nop
	ret
2:
	movl	$0x0100, %eax
	pause
	nop
	nop
	cmpb	$0, (%edx)
	je	1b
	jmp	2b

#else
#error "unknown cpu"
#endif


ENTRY(__cpu_simple_unlock)
#if defined(__amd64)
	movb	$0, (%rdi)
	ret
#elif defined(__i386)
	movl	4(%esp), %edx
	movb	$0, (%edx)
	ret
#else
#error "unknown cpu"
#endif


ENTRY(__cpu_simple_lock_try)
#if defined(__amd64)
	movl	$0x0100, %eax
	lock
	cmpxchgb %ah, (%rdi)
	movl	$0, %eax
	setz	%al
	ret
	nop
	nop
	ret

#elif defined(__i386)
	movl	4(%esp), %edx
	movl	$0x0100, %eax
	lock
	cmpxchgb %ah, (%edx)
	movl	$0, %eax
	setz	%al	
	ret
	nop
	nop
	ret

#else
#error "unknown cpu"
#endif
