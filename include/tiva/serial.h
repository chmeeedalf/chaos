#include <chaos/device.h>
#include <mach/gpio.h>
#include <gpio.h>

#ifndef TIVA_SERIAL_H
#define TIVA_SERIAL_H

namespace tiva {
struct serial_softc {
	int uart;
	int tx_pin;
	int rx_pin;
	int cts_pin;
	int rts_pin;
	int dcd_pin;
	int dsr_pin;
	int dtr_pin;
	int ri_pin;

	int parity;
	int baudrate;
};

class serial : public chaos::device<serial_softc,chaos::root_device>{
	public:
	using device<serial_softc,chaos::root_device>::device;
	virtual int init() const;
	virtual int destroy() const;
	virtual int probe() const;
	virtual int show() const;
	virtual int cdev_read(char *, int) const;
	virtual int cdev_write(const char *, int) const;
	int configure(int baud, int bits, int parity, int stop);
};

}

#endif
