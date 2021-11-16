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

#include <stdint.h>
#include <stdio.h>
#include <drivers/ds18b20_1w.h>

#define DS18B20_CONVERT	0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

namespace chaos {
int ds18b20::get_temp() const
{
	auto parent = this->parent()->as_bus(std::type_identity<onewire_bus>{});
	uint8_t data[9];
	int temp;

	parent->w1_skip_rom();
	parent->w1_write(DS18B20_CONVERT);

	while (parent->w1_read() == 0)
		;
	parent->w1_skip_rom();
	parent->w1_write(DS18B20_READ_SCRATCHPAD);

	for (int i = 0; i < 9; i++)
		data[i] = parent->w1_read();

	temp = (data[1] << 8) | data[0];

	return (temp);
}

int
ds18b20::show() const
{
	int temp = get_temp();

	iprintf("Temperature: %d\r\n", temp >> 4);
	onewire_device::show();
	return (0);
}

}
