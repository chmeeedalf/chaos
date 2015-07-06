#include <chaos/device.h>
#include <drivers/i2c.h>

namespace tiva {

struct i2c_bus_softc : chaos::i2c_bus_softc {
	enum {
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
	} bus;
};

class i2c_bus :	public chaos::i2c_bus<tiva::i2c_bus_softc,chaos::root_device> {
	public:
	using chaos::i2c_bus<tiva::i2c_bus_softc,chaos::root_device>::i2c_bus;
	virtual int i2c_write(uint8_t addr, uint8_t *data, int len) const;
	virtual int i2c_read(uint8_t addr, uint8_t *data, int len) const;
	protected:
	virtual int init() const;
	virtual void i2c_start(uint8_t addr, bool recv) const;
	virtual void i2c_restart() const;
	virtual void i2c_stop() const;
	virtual uint8_t i2c_read_byte() const;
	virtual void i2c_write_byte(uint8_t) const;
};

}
