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
#ifndef _SPI_H_
#define _SPI_H_

#include <chaos/device.h>
#include <chaos/lock.h>

namespace chaos {

class spi_device;
class spi_bus	:	public virtual device {
	// Data types
public:
	struct spi_transfer {
		const uint8_t *tx_buffer;
		uint8_t *rx_buffer;
		size_t len;
		int error;
	};

	// Functions
protected:
	spi_bus() {}
public:
	void add_child(spi_device *child);
	virtual void transfer(spi_device *slave, spi_transfer *xfer) = 0;

	// Member variables
protected:
	lock sem;
};

class spi_device	: public virtual device {
protected:
	int32_t chip_select;
	// Protected constructor, dummy device constructor.
	// Calls device constructor with nullptr arguments because this is an
	// abstract class, and subclasses are expected to construct device
	// *before* constructing spi_device.
	spi_device(int32_t chip_select) : device(nullptr, nullptr) {}
public:
	int32_t chipselect() const { return (chip_select); }
};
}

#endif
