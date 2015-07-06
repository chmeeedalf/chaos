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

namespace chaos {

typedef void (*thread_func_t)(void *);

enum {
    THREAD_STATE_IDLE = 0,
    THREAD_STATE_RUNNING,
    THREAD_STATE_SUSPENDED,
    THREAD_STATE_WAITING,
};

class thread;

/*
 * A thread is a context of execution.  It may or may not belong to a task.
 *
 * Threads can be either real-time or non-real-time.  Real-time threads are
 * run-to-completion (or event-based), while non-real-time threads are
 * preemptible, if the system is configured as such.
 */
struct thread {

	struct run : public chaos::list<chaos::thread::run>::node {
		//friend class thread;
		const thread	*thr_thread;
		struct _reent	 thr_reent = {};
		int		 thr_tid = 0;
		time_t		 thr_runtime = 0;
		time_t		 thr_lastrun = 0;
		int		 thr_state = 0;
		ssize_t		 thr_heap_top = 0;
		public:
			constexpr run(const thread *thread) : thr_thread(thread) {}
	};

	const char	*thr_name;
	thread_func_t	 thr_entry;
	uint32_t	 thr_deadline;	/* In microseconds, deadline after which
					   this thread has failed. */
	uint32_t	 thr_priority;
	run	*thr_run;
	size_t		 thr_ssize;
	size_t		 thr_hsize;
	uintptr_t	*thr_stack;
	uintptr_t	*thr_heap;	/* Pointer. */

	// Class methods
	static thread *find_by_tid(int tid);

	// Instance methods
	void start() const;
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
	extern chaos::thread::run __CONCAT(thrname,__run); \
	uintptr_t __CONCAT(thrname,__stack)[KERN_ROUND(ssize,sizeof(uintptr_t))]; \
	uintptr_t __CONCAT(thrname,__heap)[KERN_ROUND(hsize,sizeof(uintptr_t))]; \
	const chaos::thread thrname __attribute__((section(".threads"))) = { \
		.thr_name = name, \
		.thr_entry = entry, \
		.thr_deadline = deadline, \
		.thr_priority = prio, \
		.thr_run = &__CONCAT(thrname,__run), \
		.thr_ssize = sizeof(__CONCAT(thrname,__stack)),\
		.thr_hsize = sizeof(__CONCAT(thrname,__heap)),\
		.thr_stack = __CONCAT(thrname,__stack), \
		.thr_heap = __CONCAT(thrname,__heap) \
	}; \
	chaos::thread::run __CONCAT(thrname,__run) { &thrname }; \
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

