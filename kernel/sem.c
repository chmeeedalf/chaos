/*
 * Copyright (c) 2014	Justin Hibbits
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
 * 3. Neither the name of the Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stdatomic.h>
#include <stddef.h>
#include <errno.h>
#include <sys/thread.h>

typedef void *sys_priority_list_t;
typedef struct sem_s {
	sys_priority_list_t waiters;
	void *token;
	_Atomic(int) count;
} *sem_t;

extern sem_t sys_sem_begin, sys_sem_end;

sem_t sem_create(void *token)
{
}

sem_t sem_init(sem_t sem)
{
	if (sem < sys_sem_begin || sem > sys_sem_end)
		return (sem_t)NULL;
}

void sem_signal(sem_t sem)
{
	sem->count++;
	sched_wake(sem->waiters);
}

void sem_wait(sem_t sem)
{
	while (sem_trywait(sem, UINT32_MAX) == EAGAIN)
		;
}

int sem_trywait(sem_t sem, uint32_t timeout)
{
	return EAGAIN;
}
