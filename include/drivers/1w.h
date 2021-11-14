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

#ifndef _1W_H_
#define _1W_H_

#include <sys/types.h>
#include <chaos/device.h>

namespace chaos {
typedef struct {
	uint8_t bytes[8];
} w1_addr_t;

/**
 * quick overview of 1-wire commands
 * READ_ROM - reads the 8-byte device signature (only one device on bus!)
 * SKIP_ROM - selects only one device on bus
 * MATCH_ROM - selects the device by signature (multiple devices on bus)
 * SEARCH_ROM - enumerates the devices on a bus.
 */
class onewire_bus : public virtual device {
public:
	using device::as_bus;
	virtual const onewire_bus *as_bus(std::type_identity<onewire_bus>) const { return this; }
	virtual void w1_reset() const = 0;
	virtual int w1_read() const = 0;
	virtual int w1_triplet(int dir) const = 0;
	virtual void w1_write(uint8_t data) const = 0;
	/* reads rom (family, addr, crc) from one device, in case of multiple this command is an error */
	int w1_read_rom(w1_addr_t *addr) const;
	/* selects one device, and puts it into transport layer mode */
	int w1_match_rom(w1_addr_t addr) const;

	/* in case there's only one device on bus, skip rom is used to put it into transport layer mode */
	void w1_skip_rom() const;

	int w1_scan(w1_addr_t addrs[], int num) const;

};

class onewire_softc {
	protected:
	w1_addr_t addr;
};

class onewire_device : public virtual chaos::device {
	public:
	onewire_device(const char *name, onewire_bus *parent) :
		device(name, parent) {}
};
}


#endif
