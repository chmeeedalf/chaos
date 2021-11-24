/*
 * Copyright (c) 2015,2021	Justin Hibbits
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
#include <stdint.h>
#include <stdio.h>
#include <drivers/ds18b20_1w.h>
#include <util/shell.h>

#define DS18B20_CONVERT			0x44
#define	DS18B20_COPY_SCRATCHPAD		0x48
#define	DS18B20_WRITE_SCRATCHPAD	0x4E
#define DS18B20_READ_SCRATCHPAD		0xBE

namespace chaos {
int
ds18b20::get_temp() const
{
	auto parent = this->parent()->as_bus(std::type_identity<onewire_bus>{});
	uint8_t data[9];
	int temp;

	parent->w1_match_rom(addr);
	parent->w1_write(DS18B20_CONVERT);

	while (parent->w1_read() == 0)
		;
	parent->w1_match_rom(addr);
	parent->w1_write(DS18B20_READ_SCRATCHPAD);

	for (int i = 0; i < 9; i++)
		data[i] = parent->w1_read();

	temp = (data[1] << 8) | data[0];

	return (temp);
}

void
ds18b20::set_precision(ds18b20::precision prec) const
{
	auto parent = this->parent()->as_bus(std::type_identity<onewire_bus>{});
	uint8_t data[9];

	parent->w1_match_rom(addr);
	parent->w1_write(DS18B20_READ_SCRATCHPAD);

	for (int i = 0; i < 5; i++)
		data[i] = parent->w1_read();

	int v = prec;
	data[4] = v << 5;

	parent->w1_match_rom(addr);
	parent->w1_write(DS18B20_WRITE_SCRATCHPAD);

	for (int i = 2; i < 5; i++)
		parent->w1_write(data[i]);
	parent->w1_match_rom(addr);
	parent->w1_write(DS18B20_COPY_SCRATCHPAD);

}

int
ds18b20::show() const
{
	int temp = get_temp();

	iprintf("Temperature: %d\r\n", temp >> 4);
	onewire_device::show();
	return (0);
}

int
ds18b20_set_precision(const char *name, int prec)
{
	const device *d = device::find_device(name);
	ds18b20::precision p;

	if (d == nullptr) {
		iprintf("No such device %s\n\r", name);
		return (ENXIO);
	}
	const ds18b20 *ds = dynamic_cast<const ds18b20 *>(d);
	if (ds == nullptr) {
		iprintf("%s is not a ds18b20!\n\r", d->name());
		return (ENXIO);
	}
	switch (prec) {
	case 9:
		p = ds18b20::precision::PREC_9_BIT;
		break;
	case 10:
		p = ds18b20::precision::PREC_9_BIT;
		break;
	case 11:
		p = ds18b20::precision::PREC_9_BIT;
		break;
	case 12:
		p = ds18b20::precision::PREC_9_BIT;
		break;
	default:
		iprintf("Invalid precision %d\n\r", prec);
		return (EINVAL);
	}
	ds->set_precision(p);
	return (0);
}

CMD_FAMILY(ds18b20);
CMD(ds18b20, set_precision, ds18b20_set_precision);

}
