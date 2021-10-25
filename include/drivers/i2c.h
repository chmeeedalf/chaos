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

#ifndef _I2C_H_
#define _I2C_H_

#include <sys/types.h>
//#include <sem.h>
#include <chaos/device.h>


namespace chaos {
struct i2c_transfer {
	uint8_t addr; /* this one already includes r/w bit */
	uint8_t *data;
	int len;
};

class i2c_bus : public device {
	protected:
	void *priv;
	int speed; /* in Hz */
	//	struct sem sem; /* protects against concurrent usage */

	struct i2c_transfer *current_transfer;
	int transfers_to_go;
	int pos;
	int xfer_error;
	//	struct sem xfer_sem; /* up when transfers are finished */

	public:

	using device::device;
	virtual int i2c_write(uint8_t addr, uint8_t *data, int len) const = 0;
	virtual int i2c_read(uint8_t addr, uint8_t *data, int len) const = 0;
	virtual int i2c_write_read(uint8_t addr, uint8_t *wdata, int wlen, uint8_t *rdata, int rlen) const { return 0; }
	protected:
	/* called by driver state handler */
	void i2c_state_machine(int state);

	virtual int init() const = 0;
	virtual void i2c_start(uint8_t addr, bool recv) const = 0;
	virtual void i2c_restart() const = 0;
	virtual void i2c_stop() const = 0;
	virtual uint8_t i2c_read_byte() const = 0;
	virtual void i2c_write_byte(uint8_t) const = 0;
};

class i2c_device : public device {
	protected:
	uint8_t addr;
	public:
	using device::device;
	uint8_t bus_addr() const { return addr; }
};

}

#endif
