/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <drivers/1w.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>


/*
int w1_reset(device w1)
{
	return w1->w1_reset(w1);
}
*/

#define W1_READ_ROM    0x33
#define W1_MATCH_ROM   0x55
#define W1_SKIP_ROM    0xcc
#define W1_SEARCH_ROM  0xf0
/* todo overdrive */

/**
 * quick overview of 1-wire commands
 * READ_ROM - reads the 8-byte device signature (only one device on bus!)
 * SKIP_ROM - selects only one device on bus
 * MATCH_ROM - selects the device by signature (multiple devices on bus)
 * SEARCH_ROM - enumerates the devices on a bus.
 */


/* reversed crc8 with 0x131 polynomial, used in 1-wire */
static uint8_t crc8r(uint8_t *p, int len)
{
	int i, j;
	/* (1) 0011 0001 = 0x31 -> 1000 1100 (1) 0x8c */
	const uint8_t g = 0x8c;
	uint8_t t = 0;

	for (j=0; j<len; j++) {
		t ^= p[j];
		for (i=0; i<8; i++) {
			if (t & 0x01)
				t = (t>>1) ^ g;
			else
				t >>= 1;
		}
	}
	return t;
}

/* reads rom (family, addr, crc) from one device, in case of multiple this command is an error */
int chaos::onewire_bus::read_rom(w1_addr_t *addr) const
{
	int r;
	int i;

	r = this->reset();
	if (r < 0) {
		return r;
	}

	r = this->write(W1_READ_ROM);
	if (r < 0) {
		return r;
	}

	for (i=0; i<sizeof(addr->bytes); i++) {
		r = this->read();
		if (r < 0) {
			return r;
		}
		addr->bytes[i] = r;
	}

	/* this means there's more than one device on wire */
	if (crc8r(addr->bytes, sizeof(addr->bytes)) != 0)
		return -EINVAL;

	return 0;
}

/* selects one device, and puts it into transport layer mode */
int chaos::onewire_bus::match_rom(w1_addr_t addr) const
{
	int r;
	int i;

	r = this->reset();
	if (r < 0) {
		return r;
	}

	r = this->write(W1_MATCH_ROM);
	if (r < 0) {
		return r;
	}

	for (i=0; i<sizeof(addr.bytes); i++) {
		r = this->write(addr.bytes[i]);
		if (r < 0) {
			return r;
		}
	}

	return 0;
}

/* in case there's only one device on bus, skip rom is used to put it into transport layer mode */
int chaos::onewire_bus::skip_rom() const
{
	int r;

	this->reset();
	return this->write(W1_SKIP_ROM);
}

int chaos::onewire_bus::scan(w1_addr_t addrs[], int num) const
{
	int current = 0;
	w1_addr_t last_addr;
	int last_conflict = 64;
	memset(last_addr.bytes, 0, sizeof(last_addr.bytes));

	do {
		int r;
		int bit;
		w1_addr_t addr;
		int conflict = 64;
		memset(addr.bytes, 0, sizeof(addr.bytes));

		this->reset();

		this->write(W1_SEARCH_ROM);
		if (r < 0) {
			return r;
		}

		for (bit=0; bit<64; bit++) {
			int b = (last_addr.bytes[bit/8] >> (bit%8)) & 1;
			if (last_conflict == bit)
				b = 1;
			
			this->triplet(b);
			if (r < 0) {
				return r;
			}
			addr.bytes[bit/8] |= (r&1) << (bit%8);

			/* b == 1 means the conflict path was already taken */
			if (b == 0 && (r & 2))
				conflict = bit;
		}

		memcpy(&last_addr, &addr, sizeof(addr));
		if (current < num && crc8r(addr.bytes, sizeof(addr.bytes)) == 0)
			memcpy(&addrs[current++], &addr, sizeof(addr));

		last_conflict = conflict;

	} while (last_conflict != 64);

	return current;
}
