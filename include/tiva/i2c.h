#include <chaos/device.h>
#include <drivers/i2c.h>

namespace tiva {

class i2c_bus :	public chaos::i2c_bus {
	static const uint32_t i2c_base[];
	static const uint32_t i2c_periph[];
	public:
	enum bus {
		I2C0,
		I2C1,
		I2C2,
		I2C3,
		I2C4,
		I2C5,
		I2C6,
		I2C7,
		I2C8,
		I2C9
	};
	i2c_bus(const char *name, bus busid) :
	    chaos::i2c_bus(name, chaos::root_bus), bus_id(busid) {}
	virtual int i2c_write(uint8_t addr, uint8_t *data, int len) const;
	virtual int i2c_read(uint8_t addr, uint8_t *data, int len) const;
	protected:
	bus bus_id;
	virtual int init() const;
	virtual void i2c_start(uint8_t addr, bool recv) const;
	virtual void i2c_restart() const;
	virtual void i2c_stop() const;
	virtual uint8_t i2c_read_byte() const;
	virtual void i2c_write_byte(uint8_t) const;
};

}
