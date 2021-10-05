/*
 * Copyright (c) 2021	Justin Hibbits
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

#ifndef	_CHAOS_LOCK_H_
#define	_CHAOS_LOCK_H_

namespace chaos {
#include <chaos/_lock.h>

class lock : public _chaos_lock {
	public:
	lock()
	{ handle = xSemaphoreCreateMutexStatic(&_lock); }

	void take_lock(void) { xSemaphoreTake(handle, -1); }
	bool try_lock(void) { return xSemaphoreTake(handle, 0); }

	void release(void) { xSemaphoreGive(handle); }
};

class recursive_lock : public _chaos_lock {
	public:
	recursive_lock()
	{ handle = xSemaphoreCreateRecursiveMutexStatic(&_lock); }

	void take_lock(void) { xSemaphoreTakeRecursive(handle, -1); }
	bool try_lock(void) { return xSemaphoreTakeRecursive(handle, 0); }

	void release(void) { xSemaphoreGiveRecursive(handle); }
};
}
#endif
