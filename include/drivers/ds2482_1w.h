#ifndef _DS2482_1W_H_
#define _DS2482_1W_H_

#include <drivers/1w.h>
#include <drivers/i2c.h>

namespace chaos {
class ds2482_data : public i2c_device_softc {
};

class ds2482	:	public i2c_device<ds2482_data>,onewire_bus {
	public:
	/* device */
	virtual int init() const;
	virtual int destroy() const;
	virtual int probe() const;
	virtual int show() const;

	/* onewire_bus */
	virtual int reset() const;
	virtual int triplet(int dir) const;
	virtual int read() const;
	virtual int write(uint8_t data) const;
	virtual int select() const;
	private:
	int set_read_ptr(uint8_t ptr) const;
	int chip_reset() const;
};
}

#endif
