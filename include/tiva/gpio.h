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

#ifndef	TIVA_GPIO_H
#define	TIVA_GPIO_H

#include <drivers/gpio.h>

namespace tiva {
class gpio : public chaos::gpio {
public:
	struct gpio_pin	: public chaos::gpio::gpio_pin {
		int pin_no;

		gpio_pin(const char *n, gpio *p, int pin) :
			chaos::gpio::gpio_pin(n, p), pin_no(pin) { init_pin(); }
		private:
		void init_pin();
	};

	gpio(const char *name) : chaos::gpio(name, chaos::root_bus) {}
};

extern gpio *gpio_PA;
extern gpio *gpio_PB;
extern gpio *gpio_PC;
extern gpio *gpio_PD;
extern gpio *gpio_PE;
extern gpio *gpio_PF;
extern gpio *gpio_PG;
extern gpio *gpio_PH;
extern gpio *gpio_PJ;
extern gpio *gpio_PK;
extern gpio *gpio_PL;
extern gpio *gpio_PM;
extern gpio *gpio_PN;
extern gpio *gpio_PP;
extern gpio *gpio_PQ;
extern gpio *gpio_PR;
extern gpio *gpio_PS;
extern gpio *gpio_PT;

}

#endif
