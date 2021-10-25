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

#include <sys/cdefs.h>
#include <chaos/bsp.h>
#include <string.h>

extern "C" {
void main(void);
void _start(void);
void chaos_systick(void);
void __libc_init_array(void);
extern uint32_t _etext, _data, _edata, _bss, _ebss;


// FreeRTOS bits
void vPortSVCHandler(void);
void xPortPendSVHandler(void);
}


namespace arm {
namespace cortex_m4 {


void hard_fault(void);
void default_isr(void);

const size_t init_stack_size = 512;
uint32_t init_stack[init_stack_size];
const uint32_t *init_stack_top = &init_stack[init_stack_size - 1];
namespace hw_reg {
/*
 * Architecture hardware registers.
 */


class hw_reg {
	uint32_t *reg;
	public:
	hw_reg(uintptr_t x) : reg((uint32_t *)x) {}
};

const hw_reg FAULTADDR = 0xe000e000;
}

typedef void (*arm_isr)(void);

__section(".isr_vector")
static const arm_isr __used interrupts[] = {
	(arm_isr)init_stack_top,// Initial stack pointer
	_start,			// Reset vector
	default_isr,		// NMI
	hard_fault,		// Hard fault
	nullptr,		// Memory management fault
	nullptr,		// Bus fault
	nullptr,		// Usage fault
	nullptr,		// Reserved
	nullptr,		// Reserved
	nullptr,		// Reserved
	nullptr,		// Reserved
	vPortSVCHandler,	// SVCall
	nullptr,		// Reserved
	nullptr,		// Reserved
	xPortPendSVHandler,
	chaos_systick,
};

void
hard_fault(void)
{
	while (1)
		;
}

void
default_isr(void)
{
	while (1);
}

} // namespace cortex_m4
} // namespace arm

void
_start(void)
{
	uint32_t *bssp;
	memcpy(&_data, &_etext, (uintptr_t)&_edata - (uintptr_t)&_data);

	for (bssp = &_bss; bssp != &_ebss; bssp++)
		*bssp = 0;

	if (bsp::init != nullptr)
		bsp::init();
	__libc_init_array();
	main();
}
