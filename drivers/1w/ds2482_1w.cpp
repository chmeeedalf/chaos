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

/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <drivers/ds2482_1w.h>
#include <drivers/1w.h>
#include <drivers/i2c.h>
#include <errno.h>


#define PTR_STATUS     0xf0
#define PTR_READ_DATA  0xe1
#define PTR_CONFIG     0xc3

#define CMD_DEV_RESET    0xf0
#define CMD_SET_READ_PTR 0xe1
#define CMD_WRITE_CONFIG 0xd2
#define CMD_1W_RESET     0xb4
#define CMD_1W_BIT       0x87
#define CMD_1W_WRITE     0xa5
#define CMD_1W_READ      0x96
#define CMD_1W_TRIPLET   0x78

#define STATUS_DIR (1<<7)
#define STATUS_TSB (1<<6)
#define STATUS_SBR (1<<5)
#define STATUS_RST (1<<4)
#define STATUS_LL  (1<<3)
#define STATUS_SD  (1<<2)
#define STATUS_PPD (1<<1)
#define STATUS_1WB (1<<0)

#define CONFIG_1WS (1<<3)
#define CONFIG_SPU (1<<2)
#define CONFIG_PPM (1<<1)
#define CONFIG_APU (1<<0)

#if 0
	virtual int triplet(int dir) = 0;
	virtual int i2c_read() = 0;
	virtual int i2c_write(uint8_t data) = 0;
	virtual int select() = 0;
#endif

int chaos::ds2482::chip_reset() const
{
	auto parent = this->parent()->as_bus(std::type_identity<chaos::i2c_bus>{});
	uint8_t c = CMD_DEV_RESET;
	uint8_t reply;
	int r;

	r = parent->i2c_write_read(this->addr, &c, 1, &reply, 1);
	if (r < 0)
		return r;

	if (reply & STATUS_RST)
		return 0;
	return -EINVAL;
}

int chaos::ds2482::set_read_ptr(uint8_t ptr) const
{
	auto parent = this->parent()->as_bus(std::type_identity<chaos::i2c_bus>{});
	uint8_t c[2];
	int r;

	c[0] = CMD_SET_READ_PTR;
	c[1] = ptr;
	r = parent->i2c_write(this->addr, c, 2);
	if (r < 0)
		return r;

	return 0;
}

static int ds2482_write_config(chaos::ds2482 *self, uint8_t config)
{
	auto parent = self->parent()->as_bus(std::type_identity<chaos::i2c_bus>{});
	uint8_t c[2];
	int r;

	c[0] = CMD_WRITE_CONFIG;
	c[1] = ~config << 4 | config;
	r = parent->i2c_write(self->bus_addr(), c, 2);
	if (r < 0)
		return r;

	return 0;
}

int chaos::ds2482::w1_reset() const
{
	auto parent = this->parent()->as_bus(std::type_identity<chaos::i2c_bus>{});
	uint8_t c = CMD_1W_RESET;
	int r;

	r = parent->i2c_write(addr, &c, 1);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = parent->i2c_read(addr, &c, 1);
		if (r < 0)
			return r;
		if ((c & STATUS_1WB) == 0)
			break;
	}
	return 0;
}

// bit == 1 also means i2c_read time slot
static int ds2482_1w_single_bit(chaos::ds2482 *master, int bit)
{
	auto parent = master->parent()->as_bus(std::type_identity<chaos::i2c_bus>{});
	uint8_t c[2] = { CMD_1W_BIT, static_cast<uint8_t>(bit<<7) };
	int r;

	r = parent->i2c_write(master->bus_addr(), c, 2);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = parent->i2c_read(master->bus_addr(), c, 1);
		if (r < 0)
			return r;
		if ((c[0] & STATUS_1WB) == 0)
			break;
	}
	return !!(c[0] & STATUS_SBR);
}

int chaos::ds2482::w1_write(uint8_t data) const
{
	auto parent = this->parent()->as_bus(std::type_identity<chaos::i2c_bus>{});
	uint8_t c[2] = { CMD_1W_WRITE, data };
	int r;

	r = parent->i2c_write(this->addr, c, 2);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = parent->i2c_read(this->addr, c, 1);
		if (r < 0)
			return r;
		if ((c[0] & STATUS_1WB) == 0)
			break;
	}
	return 0;
}

int chaos::ds2482::w1_read() const
{
	auto parent = this->parent()->as_bus(std::type_identity<chaos::i2c_bus>{});
	uint8_t c = CMD_1W_READ;
	int r;

	r = parent->i2c_write(this->addr, &c, 1);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = parent->i2c_read(this->addr, &c, 1);
		if (r < 0)
			return r;
		if ((c & STATUS_1WB) == 0)
			break;
	}

	r = set_read_ptr(PTR_READ_DATA);
	if (r < 0)
		return r;

	r = parent->i2c_read(this->addr, &c, 1);
	if (r < 0)
		return r;

	return c;
}

/* dir - 0/1 direction to take in case of device conflict */
int chaos::ds2482::w1_triplet(int dir) const
{
	auto parent = this->parent()->as_bus(std::type_identity<chaos::i2c_bus>{});
	uint8_t c[2] = { CMD_1W_TRIPLET, static_cast<uint8_t>(dir<<7) };
	int r;

	r = parent->i2c_write(this->addr, c, 2);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = parent->i2c_read(this->addr, c, 1);
		if (r < 0)
			return r;
		if ((c[0] & STATUS_1WB) == 0)
			break;
	}

	/* return value:
	 * bit 0: path taken
	 * bit 1: conflict?
	 */
	int bit1 = !!(c[0] & STATUS_SBR);
	int bit2 = !!(c[0] & STATUS_TSB);

	/* error, no device pulled the bit low */
	if (bit1 == 1 && bit2 == 1)
		return -ENODEV;

	dir = !!(c[0] & STATUS_DIR);

	/* conflict */
	if (bit1 == 0 && bit2 == 0)
		return 2 | dir;
	return dir;
}

int chaos::ds2482::init() const
{
	int r = w1_reset();
	if (r < 0)
		return r;
	return 0;
}
