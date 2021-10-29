#ifndef _DS2482_1W_H_
#define _DS2482_1W_H_

#include <drivers/1w.h>
#include <drivers/i2c.h>

namespace chaos {

class ds2482	:	public i2c_device, public onewire_bus {
	public:
	/* device */
	virtual int init() const;
	//virtual int show() const;

	/* onewire_bus */
	virtual int w1_reset() const;
	virtual int w1_triplet(int dir) const;
	virtual int w1_read() const;
	virtual int w1_write(uint8_t data) const;
	private:
	int set_read_ptr(uint8_t ptr) const;
	int chip_reset() const;
};
}

#endif
