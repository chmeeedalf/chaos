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

#include <reent.h>
#include <errno.h>
#include <sys/lock.h>
#include <sys/types.h>
#include <chaos/thread.h>

extern "C" {
long __stack_chk_guard[8] = {0};

void
__stack_chk_fail(void)
{
	*(volatile uint32_t *)0 = 0; /* Force a hard fault */
}
}

using chaos::curthread;

void
*_sbrk_r(struct _reent *reent, ptrdiff_t diff)
{
	void *ret;

	diff = KERN_ROUND(diff, sizeof(uint32_t));

	if (curthread->thr_run->thr_heap_top + diff < 0) {
		reent->_errno = EINVAL;
		ret = (void *)-1;
	}
	else if (curthread->thr_run->thr_heap_top + diff > (curthread->thr_hsize / sizeof(uint32_t))) {
		reent->_errno = ENOMEM;
		ret = (void *)-1;
	}
	else {
		ret = &curthread->thr_heap[curthread->thr_run->thr_heap_top];
		curthread->thr_run->thr_heap_top += diff;
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
_getreent(void)
{
	return &curthread->thr_run->thr_reent;
}
