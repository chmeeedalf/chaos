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

#ifndef SYS_SCHED_H
#define SYS_SCHED_H

#include <config.h>
#include <sys/cdefs.h>
#include <sys/reent.h>
#include <sys/types.h>
#include <chaos/kernel.h>
#include <chaos/list.h>
#include <chaos/queue.h>

#include <bsp/bsp.h>

/* FreeRTOS headers */
#include <FreeRTOS.h>
#include <task.h>

namespace chaos {

typedef void (*thread_func_t)(void *);

/*
 * A thread is a context of execution.  It may or may not belong to a task.
 *
 * Threads can be either real-time or non-real-time.  Real-time threads are
 * run-to-completion (or event-based), while non-real-time threads are
 * preemptible, if the system is configured as such.
 */
class thread {

	public:
	struct run : public chaos::list<chaos::thread::run>::node {
		friend class thread;
		StaticTask_t	 thr_base;
		const thread	*thr_thread;
		struct _reent	 thr_reent = {};
		int		 thr_tid = 0;
		time_t		 thr_runtime = 0;
		time_t		 thr_lastrun = 0;
		int		 thr_state = 0;
		ssize_t		 thr_heap_top = 0;
		uint32_t	 thr_curprio = 0;
		TaskHandle_t	 thr_handle = 0;
		public:
			run(const thread *thread);
	};

	template <size_t HS, size_t SS>
	struct static_run : run{
		static_assert(HS % sizeof(uintptr_t) == 0);
		static_assert(SS % sizeof(uintptr_t) == 0);
		static const size_t HEAP_SIZE = HS;
		static const size_t STACK_SIZE = SS;
		uintptr_t heap[HEAP_SIZE / sizeof(uintptr_t)];
		uintptr_t stack[STACK_SIZE / sizeof(uintptr_t)];
		public:
		static_run(const thread *thr) : run(thr) {}
	};

	protected:
	const char	*thr_name;
	thread_func_t	 thr_entry;
	run		*thr_run;
	size_t		 thr_ssize;
	size_t		 thr_hsize;
	uintptr_t	*thr_stack;
	uintptr_t	*thr_heap;	/* Pointer. */
	uint32_t	 thr_deadline;	/* In microseconds, deadline after which
					   this thread has failed. */
	uint32_t	 thr_priority;

	public:
	constexpr thread(void) {}
	constexpr thread(const char *name,
			thread_func_t entry,
			run *_run,
			size_t ssize,
			size_t hsize,
			uintptr_t *stack,
			uintptr_t *heap,
			uint32_t deadline = 0,
			uint32_t priority = 0) :
		thr_name(name), thr_entry(entry), thr_run(_run),
		thr_ssize(ssize), thr_hsize(hsize),
		thr_stack(stack), thr_heap(heap),
		thr_deadline(deadline), thr_priority(priority) {}

	// Class methods
	static const thread *find_by_tid(int tid);
	static const thread *current();
	static const thread *next();

	// Instance methods
	void start() const;
	void show() const;

	// Accessors
	uintptr_t *stack(void) const { return thr_stack; }
	const char *name(void) const { return thr_name; }
	int thread_id(void) const { return thr_run->thr_tid; }
	void *sbrk(ptrdiff_t) const;

	struct _reent *reent(void) const { return &thr_run->thr_reent; }

	enum {
	    NEW = 0,
	    IDLE,
	    RUNNING,
	    SUSPENDED,
	    WAITING,
	};

	// FreeRTOS methods
	void get_memory(StaticTask_t **tcb_buf, StackType_t **stackp,
		uint32_t *stack_size) const;
};

/*
 * A task runs in unprivileged mode.  It consists of a set of threads.  It can
 * be created statically (compile) or dynamically (runtime).  Dynamic creation
 * uses a task template, cloned and modified as needed.  Tasks cannot have
 * real-time threads.
 */
class task {
	public:
		class run {
			struct task_s	*ta_task;
			int		 ta_pid;
		};
		const char	*ta_name;
		run	*ta_run;
};

extern const thread *curthread;
void sched_yield(void);

thread *thread_create(thread *thr_template);
thread *thread_find_by_tid(int tid);

#define __THREAD(name, entry, deadline, ssize, hsize, prio, thrname) \
	extern chaos::thread::static_run<hsize,ssize> __CONCAT(thrname,__run); \
	static_assert(ssize >= sizeof(bsp::context), \
	    "Thread stack frame too small to hold thread context"); \
	const chaos::thread __section(".threads") thrname { \
		name, \
		entry, \
		&__CONCAT(thrname,__run), \
		sizeof(__CONCAT(thrname,__run)).stack,\
		sizeof(__CONCAT(thrname,__run)).heap,\
		__CONCAT(thrname,__run).stack, \
		__CONCAT(thrname,__run).heap, \
		deadline, \
		prio \
	}; \
	chaos::thread::static_run<hsize,ssize> __CONCAT(thrname,__run) ( &thrname ); \
	struct hack

#define NAMED_THREAD(name, descr, entry, deadline, ssize, hsize, prio) \
	__THREAD(descr, entry, deadline, ssize, hsize, prio, name)

#define _THREAD(descr, entry, deadline, ssize, hsize, prio) \
	__THREAD(descr, entry, deadline, ssize, hsize, prio, __CONCAT(thread,__COUNTER__))
#define RTTHREAD(descr, entry, deadline, ssize, hsize, prio) \
	_THREAD(descr, entry, deadline, ssize, hsize, prio)

#define THREAD(descr, entry, ssize, hsize, prio) \
	_THREAD(descr, entry, 0, ssize, hsize, prio)
#endif


/* Scheduler related. */
void sched_run(void);
void sched_tick(void);

}

