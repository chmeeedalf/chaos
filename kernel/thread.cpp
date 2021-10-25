/*
 * Copyright (c) 2015,2021	Justin Hibbits
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
#include <stdlib.h>
#include <string.h>
#include <util/shell.h>

/* FreeRTOS headers */
#include <FreeRTOS.h>
#include <task.h>

extern chaos::thread sys_threads[];
extern chaos::thread sys_threads_end;

namespace chaos {
/* Arbitrary.  Total stack+heap == 8.5kB. */
extern const thread idle;
extern const thread timers;

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

const thread *
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
	TaskStatus_t tstatus;
	/* Create the FreeRTOS structures if needed. */
	if (thr_run->thr_state == NEW) {
		thr_run->thr_handle = xTaskCreateStatic(thr_entry,
		    thr_name, thr_ssize / sizeof(long), nullptr, thr_priority,
		    (StackType_t *)thr_stack, &thr_run->thr_base);
		thr_run->thr_state = RUNNING;
		vTaskGetInfo(reinterpret_cast<TaskHandle_t>(&thr_run->thr_base),
		    &tstatus, false, eInvalid);
		vTaskSetThreadLocalStoragePointer(thr_run->thr_handle, 0,
		    const_cast<void *>(reinterpret_cast<const void *>(this)));
		thr_run->thr_tid = tstatus.xTaskNumber;
	}
}

const thread *
thread::current(void)
{
	TaskHandle_t th = xTaskGetCurrentTaskHandle();

	return reinterpret_cast<const thread *>(pvTaskGetThreadLocalStoragePointer(th, 0));
}

thread::run::run(const thread *thr)
{ 
	thr_thread = thr;
	_REENT_INIT_PTR(&thr_reent);
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

void
thread::show(void) const
{
	iprintf("Name:\t%s\n\r", thr_name);
	iprintf("State:\t%s\n\r", thread_state(thr_run->thr_state));
	iprintf("Static:\t%p\n\r", this);
	iprintf("Run:\t%p\n\r", thr_run);
	iprintf("Heap:\n\r");
	iprintf("  addr:\t%p\n\r", thr_heap);
	iprintf("  size:\t%zu\n\r", thr_hsize);
	iprintf("  used:\t%zd, %d%%\n\r", thr_run->thr_heap_top * sizeof(uintptr_t),
	    (int)((thr_run->thr_heap_top * 100) / thr_hsize));
}

void
thread::get_memory(StaticTask_t **tcb_buf, StackType_t **stackp,
		uint32_t *stack_size) const
{
	*tcb_buf = &thr_run->thr_base;
	*stackp = reinterpret_cast<StackType_t *>(thr_stack);
	*stack_size = thr_ssize / sizeof(unsigned long);
}

void *
thread::sbrk(ptrdiff_t diff) const
{
	void *ret;

	if (thr_run->thr_heap_top + diff < 0) {
		thr_run->thr_reent._errno = EINVAL;
		ret = (void *)-1;
	}
	else if (thr_run->thr_heap_top + diff > thr_hsize) {
		thr_run->thr_reent._errno = ENOMEM;
		ret = (void *)-1;
	}
	else {
		ret = &thr_heap[thr_run->thr_heap_top];
		thr_run->thr_heap_top += diff;
	}
	return ret;
}

static int
thread_show(int tid)
{
	const thread *td = thread::find_by_tid(tid);
	
	if (td == nullptr) {
		iprintf("No thread matching TID %d\n\r", tid);
		return -1;
	}

	td->show();

	return 0;
}

static int
thread_list(void)
{
	const thread *i;
	iprintf("     TID\t  NAME\n\r");
	for (i = &sys_threads[0]; i != &sys_threads_end; i++) {
		iprintf("%8d\t  %s\n\r", i->thread_id(), i->name());
	}
	for (int t = 0; t < MAX_DYNAMIC_THREADS; t++) {
		if (dynamic_threads[t].name() != nullptr)
			iprintf("%8d\t  %s\n\r", i->thread_id(), i->name());
	}
	return 0;
}

void
sched_run(void)
{
	thread_init_os();
	bsp::systick_enable();
	vTaskStartScheduler();
}

extern "C" void xPortSysTickHandler();
void
sched_tick(void)
{
	xPortSysTickHandler();
}

CMD_FAMILY(thread);
CMD(thread, show, thread_show);
CMD(thread, list, thread_list);
}

extern "C" {

__attribute__((weak)) void
_init(void)
{
}

__attribute__((weak)) void
_fini(void)
{
}

void
chaos_systick(void)
{
	chaos::sched_tick();
}

void
vApplicationGetIdleTaskMemory(StaticTask_t **tcb_buf, StackType_t **stackp,
		uint32_t *stack_size)
{
	chaos::idle.get_memory(tcb_buf, stackp, stack_size);
}

void
vApplicationGetTimerTaskMemory(StaticTask_t **tcb_buf, StackType_t **stackp,
		uint32_t *stack_size)
{
	chaos::timers.get_memory(tcb_buf, stackp, stack_size);
}
}
