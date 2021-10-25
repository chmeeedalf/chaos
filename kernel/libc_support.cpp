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

#include <errno.h>
#include <reent.h>
#include <stdlib.h>
#include <sys/lock.h>
#include <sys/types.h>
#include <chaos/thread.h>
#include <chaos/lock.h>

#include <new>

extern "C" {
long __stack_chk_guard[8] = {0};

void
__stack_chk_fail(void)
{
	*(volatile uint32_t *)0 = 0; /* Force a hard fault */
}
}

void
*_sbrk_r(struct _reent *reent, ptrdiff_t diff)
{
	void *ret;

	diff = howmany(diff, sizeof(uint32_t));

	if (reent != curthread->reent()) {
		reent->_errno = EINVAL;
		ret = (void *)-1;
	}
	else {
		ret = curthread->sbrk(diff);
	}

	return ret;
}

int
_fstat_r(struct _reent *reent, int fd, struct stat *sb)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_close_r(struct _reent *reent, int fd)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_isatty_r(struct _reent *reent, int fd)
{
	reent->_errno = ENOSYS;
	return -1;
}

off_t
_lseek_r(struct _reent *reent, int fd, off_t offset, int whence)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_read_r(struct _reent *reent, int fd, void *buf, size_t nb)
{
	reent->_errno = ENOSYS;
	return -1;
}

ssize_t
_write_r(struct _reent *reent, int fd, const void *buf, size_t nb)
{
	reent->_errno = ENOSYS;
	return -1;
}

struct _reent *
__getreent(void)
{
	return curthread->reent();
}

extern "C" {
struct _chaos_lock _lock_list, _lock_list_end;
struct _chaos_lock _recursive_lock_list, _recursive_lock_list_end;
}

static void
init_locks(void)
{
	auto l = &_lock_list;
	static_assert(sizeof(chaos::lock) == sizeof(_chaos_lock));
	for (; l < &_lock_list_end; l++)
		chaos_lock_init(l);

	for (l = &_recursive_lock_list; l < &_recursive_lock_list_end; l++)
		chaos_lock_init_recursive(l);
}

namespace chaos {
	class _init {
		public:
		_init();
		bool inited;
	};

	_init::_init() {
		init_locks();
		inited = true;
	}
	static _init init_chaos;
}


void
chaos_lock_init(_LOCK_T *lock)
{
	new(reinterpret_cast<chaos::lock *>(lock)) chaos::lock();
}

void
chaos_lock_init_recursive(_LOCK_RECURSIVE_T *lock)
{
	new(reinterpret_cast<chaos::recursive_lock *>(lock)) chaos::recursive_lock();
}

void chaos_lock_close(_LOCK_T *lock)
{
	reinterpret_cast<chaos::lock *>(lock)->~lock();
}

void
chaos_lock_close_recursive(_LOCK_RECURSIVE_T *lock)
{
	reinterpret_cast<chaos::recursive_lock *>(lock)->~recursive_lock();
}

void
chaos_lock_acquire(_LOCK_T *lock)
{
	if (!chaos::init_chaos.inited)
		return;
	reinterpret_cast<chaos::lock *>(lock)->take_lock();
}

void
chaos_lock_acquire_recursive(_LOCK_RECURSIVE_T *lock)
{
	if (!chaos::init_chaos.inited)
		return;
	reinterpret_cast<chaos::recursive_lock *>(lock)->take_lock();
}

bool
chaos_lock_try_acquire(_LOCK_T *lock)
{
	if (!chaos::init_chaos.inited)
		return true;
	return reinterpret_cast<chaos::lock *>(lock)->try_lock();
}

bool
chaos_lock_try_acquire_recursive(_LOCK_RECURSIVE_T *lock)
{
	if (!chaos::init_chaos.inited)
		return true;
	return reinterpret_cast<chaos::recursive_lock *>(lock)->try_lock();
}

void
chaos_lock_release(_LOCK_T *lock)
{
	if (!chaos::init_chaos.inited)
		return;
	reinterpret_cast<chaos::lock *>(lock)->release();
}

void
chaos_lock_release_recursive(_LOCK_RECURSIVE_T *lock)
{
	if (!chaos::init_chaos.inited)
		return;
	reinterpret_cast<chaos::recursive_lock *>(lock)->release();
}


void
vPortFree(void *ptr)
{
	free(ptr);
}
