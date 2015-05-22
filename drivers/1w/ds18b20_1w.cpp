/*
 * Copyright (c) 2014	Justin Hibbits
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
//#include <stdio.h>
#include <drivers/ds18b20_1w.h>
#include <sys/device.h>

#define DS18B20_CONVERT	0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

int ds18b20::get_temp() const
{
	uint8_t data[9];
	int temp;

	this->dev_parent->reset();
	this->dev_parent->skip_rom();
	//w1_match_rom(parent, sc->sc_addr);
	this->dev_parent->write(DS18B20_CONVERT);

	while (this->dev_parent->read() == 0)
		;
	this->dev_parent->write(DS18B20_READ_SCRATCHPAD);

	for (int i = 0; i < 9; i++)
		data[i] = this->dev_parent->read();

	temp = (data[1] << 8) | data[0];

	return (temp);
}

#if 0
static int
ds18b20_show(const ds18b20 *w1)
{
	int temp = update_temp(w1);

	iprintf("Temperature: %d\r\n", temp >> 4);
	return (0);
}
#endif

