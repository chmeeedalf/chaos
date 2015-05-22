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

#ifndef SYS_SCHED_H
#define SYS_SCHED_H

#include <sys/cdefs.h>
#include <sys/queue.h>
#include <sys/reent.h>
#include <sys/types.h>
#include <sys/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef void (*thread_func_t)(void *);

typedef struct thread_s thread_t;

enum {
    THREAD_STATE_IDLE = 0,
    THREAD_STATE_RUNNING,
    THREAD_STATE_SUSPENDED,
    THREAD_STATE_WAITING,
};
typedef struct thread_run_s {
	const thread_t	*thr_thread;
	struct _reent	 thr_reent;
	int		 thr_tid;
	time_t		 thr_runtime;
	time_t		 thr_lastrun;
	int		 thr_state;
	ssize_t		 thr_heap_top;
} thread_run_t;

/*
 * A thread is a context of execution.  It may or may not belong to a task.
 *
 * Threads can be either real-time or non-real-time.  Real-time threads are
 * run-to-completion (or event-based), while non-real-time threads are
 * preemptible, if the system is configured as such.
 */
struct thread_s {
	const char	*thr_name;
	thread_func_t	 thr_entry;
	uint32_t	 thr_deadline;	/* In microseconds, deadline after which
					   this thread has failed. */
	uint32_t	 thr_priority;
	thread_run_t	*thr_run;
	size_t		 thr_ssize;
	size_t		 thr_hsize;
	uintptr_t	*thr_stack;
	void		*thr_heap;	/* Pointer. */
};

typedef struct task_run_s {
	struct task_s	*ta_task;
	int		 ta_pid;
} task_run_t;

/*
 * A task runs in unprivileged mode.  It consists of a set of threads.  It can
 * be created statically (compile) or dynamically (runtime).  Dynamic creation
 * uses a task template, cloned and modified as needed.  Tasks cannot have
 * real-time threads.
 */
typedef struct task_s {
	const char	*ta_name;
	task_run_t	*ta_run;
} task_t;

extern const thread_t *curthread;
void sched_yield(void);

thread_t *thread_create(thread_t *thr_template);
thread_t *thread_find_by_tid(int tid);

#define __THREAD(name, entry, deadline, ssize, hsize, prio, thrname) \
	extern thread_run_t __CONCAT(thrname,__run); \
	uintptr_t __CONCAT(thrname,__stack)[KERN_ROUND(ssize,sizeof(uintptr_t))]; \
	uintptr_t __CONCAT(thrname,__heap)[KERN_ROUND(hsize,sizeof(uintptr_t))]; \
	const thread_t thrname __attribute__((section(".threads"))) = { \
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
	thread_run_t __CONCAT(thrname,__run) = { .thr_thread = &thrname }; \
	struct hack

#define _THREAD(name, entry, deadline, ssize, hsize, prio) \
	__THREAD(name, entry, deadline, ssize, hsize, prio, __CONCAT(thread,__COUNTER__))
#define RTTHREAD(name, entry, deadline, ssize, hsize, prio) \
	_THREAD(name, entry, deadline, ssize, hsize, prio)

#define THREAD(name, entry, ssize, hsize, prio) \
	_THREAD(name, entry, 0, ssize, hsize, prio)
#endif
#ifdef __cplusplus
}
#endif

