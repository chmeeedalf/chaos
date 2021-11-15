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
#ifndef _GPIO_H_
#define _GPIO_H_

#include <chaos/device.h>

namespace chaos {

class gpio : public device {
public:

	struct gpio_pin;
	friend struct gpio_pin;

	gpio(const char *name, const device *parent) : device(name, parent) {}
	virtual bool state(gpio_pin *p) = 0;
	virtual void configure(gpio_pin *p) = 0;
	struct gpio_pin {
		enum class gpio_dir {
			INPUT,
			OUTPUT
		};
		enum class gpio_mode {
			GPIO_INPUT_PU,
			GPIO_INPUT_PD,
			GPIO_OUTPUT_SLOW,
			GPIO_ANALOG
		};
		enum class gpio_trigger {
			LEVEL,
			EDGE
		};
		enum class gpio_edge {
			LOW,
			HIGH,
			BOTH
		};
		enum class gpio_pull_up_down {
			NONE,
			PULLUP,
			PULLDOWN
		};

		protected:
		gpio_pin(const char *n, chaos::gpio *gpio) :
			port(gpio), name(n) {}
		chaos::gpio *port;
		const char *name;	// May be NULL if unnamed

		public:

		// These are bitfields to reduce the size of the structure,
		// since there are so many tiny fields here.
		gpio_edge edge			:2;
		gpio_pull_up_down updown	:2;
		gpio_dir direction		:1;
		gpio_mode mode			:1;
		gpio_trigger trigger		:1;
		bool open_drain			:1;

		bool state(void) {
			return port->state(this);
		}
		void set(bool state) {
			port->set_output(this, state);
		}
		void set_direction(gpio_dir d) {
			port->set_direction(this, d);
		}
		void configure(void) {
			port->configure(this);
		}
	};
protected:
	virtual void set_direction(gpio_pin *p, gpio_pin::gpio_dir direction) = 0;
	virtual void set_output(gpio_pin *p, bool state) = 0;
};

}

#endif
