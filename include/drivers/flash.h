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

#ifndef	DRIVERS_FLASH_H
#define	DRIVERS_FLASH_H
#include <sys/types.h>
#include <stdint.h>

namespace chaos {

/**
 * Direct-address flash memory storage abstract base class.
 *
 * Subclasses are expected to provide the following static methods:
 * \li \code erase
 * \li \code write
 */
template <size_t EB, size_t BC, uintptr_t B>
class flash {
public:
	// Class variables
	static constexpr size_t block_count = BC;
	static constexpr size_t erase_size = EB;
	static constexpr size_t flash_size = block_count * erase_size;
	static constexpr uintptr_t flash_base = B;
	static constexpr uintptr_t flash_end = flash_base + flash_size;

	// These *must* be implemented in subclasses, as static classes
	static void erase(uintptr_t base);
	static void write(uintptr_t target, uintptr_t src, size_t bytes);
};
}
#endif
