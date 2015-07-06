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

#include <limits.h>
#include <stdint.h>

#ifndef _SYS_QUEUE_H
#define _SYS_QUEUE_H

namespace chaos {
/*!
 * This is a simple queue: you can perform the following operations:
 * - push one or many (sleep until there's room)
 * - try_push one or many (sleep for X ticks, fail if it's full)
 * - pop one or many (sleep until there's enough)
 * - try_pop one or many (sleep for X ticks, fail if there's not enough)
 * - count the items in the queue
 *
 * This will form the basis of the semaphore as well.
 */
template <typename T,int Q>
class queue {
	int chead;
	int ctail;
	T data[Q];

	public:
	queue() : chead(0), ctail(0), data() {}
	void push(T);
	void push(int, T *);
	bool try_push(T, int);
	bool try_push(int, T *, int);

	T pop(void);
	void pop(int, T *);
	bool try_pop(T *, int);
	bool try_pop(int, T *, int);
	int count();
};

template <typename T, int Q>
void queue<T,Q>::push(T val)
{
	while (!try_push(1, &val, INT_MAX))
		;
}

template <typename T, int Q>
void queue<T,Q>::push(int count, T *vals)
{
	while (!try_push(count, vals, INT_MAX))
		;
}

template <typename T, int Q>
bool queue<T,Q>::try_push(T val, int timeout)
{
	return try_push(1, &val, timeout);
}

template <typename T, int Q>
bool queue<T,Q>::try_push(int count, T *vals, int timeout)
{
	if (count > Q)
		return false;
}

template <typename T, int Q>
T queue<T,Q>::pop()
{
	T val;

	while (!try_pop(1, &val, INT_MAX))
		;
	return val;
}

template <typename T, int Q>
void queue<T,Q>::pop(int count, T *vals)
{
	while (!try_pop(count, vals, INT_MAX))
		;
}

template <typename T, int Q>
bool queue<T,Q>::try_pop(T *val, int timeout)
{
	return try_pop(1, val, timeout);
}

template <typename T, int Q>
bool queue<T,Q>::try_pop(int count, T *vals, int timeout)
{
	if (count > Q)
		return false;
}

template <typename T, int Q>
int queue<T,Q>::count()
{
	return (ctail + Q - chead) % (Q + 1);
}

}

#endif
