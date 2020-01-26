/*
 * Copyright (c) 2015	Justin Hibbits
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
#include <chaos/bsp.h>
#include <chaos/list.h>
#include <chaos/thread.h>

#include <stdio.h>
#include <util/shell.h>

extern chaos::thread sys_threads[];
extern chaos::thread sys_threads_end;

namespace chaos {
/* Arbitrary.  Total stack+heap == 8.5kB. */
NAMED_THREAD(chaos_kernel, "chaos kernel",nullptr,0,512,8192,0);

const thread *curthread = &chaos_kernel;
static thread dynamic_threads[MAX_DYNAMIC_THREADS];

static list<thread::run> run_queue;

void
thread_init_os(void)
{
	bsp::set_systick(CHAOS_SCHED_TICK);
}

thread *
thread_create(thread *thr_template)
{
	return nullptr;
}

thread *
thread::find_by_tid(int tid)
{
	thread *td = &sys_threads[0];

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

void
thread::start(void) const
{
}

static const char *
thread_state(int state)
{
	switch (state) {
	    default: return "Invalid";
	    case thread::IDLE: return "Idle";
	    case thread::RUNNING: return "Running";
	    case thread::SUSPENDED: return "Suspended";
	    case thread::WAITING: return "Waiting";
	};
}

static int
thread_show(int tid)
{
	thread *td = thread::find_by_tid(tid);
	
	if (td == nullptr) {
		iprintf("No thread matching TID %d\n\r", tid);
		return -1;
	}

	iprintf("Name:\t%s\n\r", td->thr_name);
	iprintf("State:\t%s\n\r", thread_state(td->thr_run->thr_state));
	iprintf("Static:\t%p\n\r", td);
	iprintf("Run:\t%p\n\r", td->thr_run);
	iprintf("Heap:\n\r");
	iprintf("  addr:\t%p\n\r", td->thr_heap);
	iprintf("  size:\t%zu\n\r", td->thr_hsize);
	iprintf("  used:\t%zd, %ld%%\n\r", td->thr_run->thr_heap_top,
	    td->thr_run->thr_heap_top * 100 / td->thr_hsize);
	return 0;
}

static int
thread_list(void)
{
	thread *i;
	iprintf("     TID\t  NAME\n\r");
	for (i = &sys_threads[0]; i != &sys_threads_end; i++) {
		iprintf("%8d\t  %s\n\r", i->thr_run->thr_tid, i->thr_name);
	}
	for (int t = 0; t < MAX_DYNAMIC_THREADS; t++) {
		if (dynamic_threads[t].thr_name != nullptr)
			iprintf("%8d\t  %s\n\r", i->thr_run->thr_tid, i->thr_name);
	}
	return 0;
}

void
sched_run(void)
{
	thread_init_os();
	bsp::systick_enable();
}

void
sched_tick(void)
{
	iprintf("tick\n");
}

CMD_FAMILY(thread);
CMD(thread, show, thread_show);
CMD(thread, list, thread_list);
}

extern "C" {
void
chaos_systick(void)
{
	chaos::sched_tick();
}
}
