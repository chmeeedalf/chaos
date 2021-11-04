#ifndef _BITBANG_1W_H_
#define _BITBANG_1W_H_

#include <drivers/1w.h>
#include <drivers/gpio.h>

extern const struct w1_methods bitbang_1w_methods;

struct bitbang_1w_softc {
	int pin;
};

class bitbang_1w	:	public device<bitbang_1w_softc,gpio>,onewire_bus {
	virtual int init() const;
	virtual int destroy() const;
	virtual int probe() const;
	virtual int show() const;
};

#endif
