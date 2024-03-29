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

#ifndef __SYS_LIST_H__
#define __SYS_LIST_H__

#include <type_traits>
namespace chaos {
template <class T>
class list {
public:
	class node {
		friend class list<T>;
		public:
			constexpr node() { }
			T *next(void) {
			    return node_next;
			}
		protected:
			T *node_next;
			T *node_prev;
	};

	constexpr list() {
		static_assert(std::is_base_of<node, T>::value);
	}
/* Public function interface. */
	T *head(void) { return head_node; }
	void add_head(T *n) {
		n->node_next = head_node;
		n->node_prev = nullptr;
		head_node->node_prev = n;
		head_node = n;
	}
	T &pop_head() {
		T *n = head_node;
		head_node = n->node_next;
		head_node->node_prev = nullptr;
		return *n;
	}
	void add_tail(T *n) {
		if (tail_node != nullptr)
			tail_node->node_next = n;
		n->node_prev = tail_node;
		tail_node = n;
		n->node_next = nullptr;
		if (head_node == nullptr)
			head_node = n;
	}
	T &pop_tail() {
		T *n = head_node;
		head_node = n->node_next;
		head_node->node_prev = nullptr;
	}

	void insert(T *n, T *before)
	{ // Insert after the 'before' node.
	}
	void remove(T *at)
	{
		// CRITICAL ENTER
		if (at == head_node)
			head_node = head_node->node_next;
		else
			at->node_prev->node_next = at->node_next;
		if (at == tail_node)
			tail_node = tail_node->node_prev;
		else
			at->node_next->node_prev = at->node_prev;
		// CRITICAL EXIT
	}

private:
	T *head_node;
	T *tail_node;
};
}
#endif
