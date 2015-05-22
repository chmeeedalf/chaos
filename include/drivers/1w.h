#ifndef _1W_H_
#define _1W_H_

#include <sys/types.h>
#include <sys/device.h>

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
class onewire_bus {
	public:
	virtual int reset() const = 0;
	virtual int triplet(int dir) const = 0;
	virtual int read() const = 0;
	virtual int write(uint8_t data) const = 0;
	virtual int select() const = 0;
	/* reads rom (family, addr, crc) from one device, in case of multiple this command is an error */
	int read_rom(w1_addr_t *addr) const;
	/* selects one device, and puts it into transport layer mode */
	int match_rom(w1_addr_t addr) const;

	/* in case there's only one device on bus, skip rom is used to put it into transport layer mode */
	int skip_rom() const;

	int scan(w1_addr_t addrs[], int num) const;

};

class onewire_softc {
	protected:
	w1_addr_t addr;
};

class onewire_device : public chaos::device<onewire_softc, onewire_bus> {
};
}


#endif
