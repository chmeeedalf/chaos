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

#include <config.h>
#include <sys/thread.h>

#include <stdio.h>
#include <util/shell.h>

/* Arbitrary.  Total stack+heap == 2kB. */
__THREAD("chaos_kernel",nullptr,0,512,10240,0,chaos_kernel);

const thread_t *curthread = &chaos_kernel;
static thread_t dynamic_threads[MAX_DYNAMIC_THREADS];
extern thread_t sys_threads[];
extern thread_t sys_threads_end;

void
thread_init_os(void)
{
}

thread_t *
thread_create(thread_t *thr_template)
{
	return nullptr;
}

thread_t *
thread_find_by_tid(int tid)
{
	thread_t *td = &sys_threads[0];

	for (; td != &sys_threads_end; td++) {
		if (td->thr_run->thr_tid == tid)
			return td;
	}

	for (int i = 0; i < MAX_DYNAMIC_THREADS; i++) {
		/* If there's no runtime struct, there's no thread. */
		if (dynamic_threads[i].thr_run == nullptr)
			continue;
		td = &dynamic_threads[i];
		if (td->thr_run->thr_tid == tid)
			return td;
	}
	return nullptr;
}

static int
thread_show(int tid)
{
	thread_t *td = thread_find_by_tid(tid);
	
	if (td == nullptr) {
		iprintf("No thread matching TID %d\n\r", tid);
		return -1;
	}

	iprintf("Name:\t%s\n\r", td->thr_name);
	return 0;
}

static int
thread_list(void)
{
	thread_t *i;
	iprintf("NAME\n");
	for (i = &sys_threads[0]; i != &sys_threads_end; i++) {
		iprintf("%s\n\r", i->thr_name);
	}
	for (int t = 0; t < MAX_DYNAMIC_THREADS; t++) {
		if (dynamic_threads[t].thr_name != nullptr)
			iprintf("%s\n\r", i->thr_name);
	}
	return 0;
}

CMD_FAMILY(thread);
CMD(thread, show, thread_show);
CMD(thread, list, thread_list);
