/*
 * Copyright (c) 2020	Justin Hibbits
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

#include <errno.h>
#include <stdio.h>
#include <drivers/gpio_1w.h>

namespace chaos {

using gpio_dir = gpio::gpio_pin::gpio_dir;

gpio_onewire::gpio_onewire(const char *name, gpio *parent, gpio::gpio_pin *pin)
	: device(name, parent), onewire_bus(name, parent)
{
	wire = pin;
	wire->open_drain = true;
	wire->direction = gpio_dir::INPUT;
	wire->configure();
}

void
gpio_onewire::w1_write_1_bit() const
{
	wire->set_direction(gpio_dir::OUTPUT);
	wire->set(0);
	udelay(2);
	wire->set_direction(gpio_dir::INPUT);
	udelay(60);
}

void
gpio_onewire::w1_write_0_bit() const
{
	wire->set_direction(gpio_dir::OUTPUT);
	wire->set(0);
	udelay(60);
	wire->set_direction(gpio_dir::INPUT);
	udelay(2);
}

int
gpio_onewire::w1_read_bit() const
{
	wire->set_direction(gpio_dir::OUTPUT);
	wire->set(0);
	udelay(2);
	wire->set_direction(gpio_dir::INPUT);
	udelay(10);
	int bit = wire->state();
	udelay(50);

	return (bit);
}

void
gpio_onewire::w1_reset() const
{
	wire->set_direction(gpio_dir::OUTPUT);
	wire->set(0);
	udelay(480);
	wire->set_direction(gpio_dir::INPUT);
	udelay(70);
	udelay(410);
}

int
gpio_onewire::w1_read() const
{
	uint8_t byte;

	byte = 0;
	for (int i = 0; i < 8; i++) {
		byte |= w1_read_bit() << i;
	}

	return (byte);
}

int
gpio_onewire::w1_triplet(int dir) const
{
	int bit1 = w1_read_bit();
	int bit2 = w1_read_bit();
	int b;

	if (bit1 ^ bit2)
		b = bit1;
	else
		b = dir;

	if (b == 1)
		w1_write_1_bit();
	else
		w1_write_0_bit();

	/* return value:
	 * bit 0: path taken
	 * bit 1: conflict?
	 */

	/* error, no device pulled the bit low */
	if (bit1 == 1 && bit2 == 1)
		return ENODEV;

	/* conflict */
	if (bit1 == 0 && bit2 == 0)
		return 2 | dir;
	return bit1;
}

void
gpio_onewire::w1_write(uint8_t data) const
{
	for (int i = 0; i < 8; i++) {
		if (data & 1)
			w1_write_1_bit();
		else
			w1_write_0_bit();
		data >>= 1;
	}
}

} // namespace chaos
