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

#ifndef TIVA_FLASH_H
#define	TIVA_FLASH_H

#include <drivers/flash.h>
#include <driverlib/flash.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>

namespace tiva {

	// Flash base is (almost?) always at 0 on Tiva MCUs.
	template <size_t EB, size_t count, uintptr_t B = 0>
	class flash	: public chaos::flash<EB, count, B> {
	public:
		static void erase(uintptr_t base) {
			MAP_FlashErase(base);
		}
		static void write(uintptr_t target, uintptr_t src,
		    size_t bytes) {
			MAP_FlashProgram((uint32_t *)src, target,
			    howmany(bytes, sizeof(uint32_t)) * sizeof(uint32_t));
		}
	};

	// TM4C129x have 16k erase sizes (interleaved 8k blocks)
	template <size_t C>
	using tm4c129x_flash = flash<16 * 1024, C>;

	// TM4C123x have 1k erase sizes
	template <size_t C>
	using tm4c123x_flash = flash<1024, C>;
};
#endif
