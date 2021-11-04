#include <chaos/device.h>
#include <mach/gpio.h>

#ifndef TIVA_SERIAL_H
#define TIVA_SERIAL_H

namespace tiva {

class serial : public chaos::char_device {
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
	public:
	serial(const char *name, int baud_rate) :
	    chaos::char_device(name, chaos::root_bus), baudrate(baud_rate)
	{init();}
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
