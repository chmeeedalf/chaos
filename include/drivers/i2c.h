#ifndef _I2C_H_
#define _I2C_H_

#include <sys/types.h>
//#include <sem.h>
#include <sys/device.h>


namespace chaos {
struct i2c_transfer {
	uint8_t addr; /* this one already includes r/w bit */
	uint8_t *data;
	int len;
};

class i2c_bus {
	public:
	int xfer(struct i2c_transfer *transfer, int num_xfers) const;
	int write(uint8_t addr, uint8_t *data, int len) const;
	int read(uint8_t addr, uint8_t *data, int len) const;
	int write_read(uint8_t addr, uint8_t *wdata, int wlen, uint8_t *rdata, int rlen) const;
	protected:
	virtual void init() const = 0;
	virtual void start() const = 0;
	virtual void restart() const = 0;
	virtual void stop() const = 0;
	virtual uint8_t read_byte() const = 0;
	virtual void write_byte(uint8_t) const = 0;
	void *priv;
	int speed; /* in Hz */
//	struct sem sem; /* protects against concurrent usage */

	struct i2c_transfer *current_transfer;
	int transfers_to_go;
	int pos;
	int xfer_error;
//	struct sem xfer_sem; /* up when transfers are finished */
};

class i2c_device_softc {
	public:
	uint8_t addr;
};

template <typename T>
class i2c_device : public device<T,i2c_bus> {
	public:
	uint8_t addr() const { return this->dev_softc->addr; }
};

/* called by driver state handler */
void i2c_state_machine(struct i2c_bus *master, int state);

/* called on master init */
void i2c_register_master(struct i2c_bus *master);

}

#endif
