/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <drivers/ds2482_1w.h>
#include <drivers/1w.h>
#include <drivers/i2c.h>
#include <errno.h>


#define PTR_STATUS     0xf0
#define PTR_READ_DATA  0xe1
#define PTR_CONFIG     0xc3

#define CMD_DEV_RESET    0xf0
#define CMD_SET_READ_PTR 0xe1
#define CMD_WRITE_CONFIG 0xd2
#define CMD_1W_RESET     0xb4
#define CMD_1W_BIT       0x87
#define CMD_1W_WRITE     0xa5
#define CMD_1W_READ      0x96
#define CMD_1W_TRIPLET   0x78

#define STATUS_DIR (1<<7)
#define STATUS_TSB (1<<6)
#define STATUS_SBR (1<<5)
#define STATUS_RST (1<<4)
#define STATUS_LL  (1<<3)
#define STATUS_SD  (1<<2)
#define STATUS_PPD (1<<1)
#define STATUS_1WB (1<<0)

#define CONFIG_1WS (1<<3)
#define CONFIG_SPU (1<<2)
#define CONFIG_PPM (1<<1)
#define CONFIG_APU (1<<0)

#if 0
	virtual int triplet(int dir) = 0;
	virtual int read() = 0;
	virtual int write(uint8_t data) = 0;
	virtual int select() = 0;
#endif

int chaos::ds2482::chip_reset() const
{
	struct chaos::ds2482_data *chip = dev_softc;
	const i2c_bus *parent = this->parent();
	uint8_t c = CMD_DEV_RESET;
	uint8_t reply;
	int r;

	r = parent->write_read(this->addr(), &c, 1, &reply, 1);
	if (r < 0)
		return r;

	if (reply & STATUS_RST)
		return 0;
	return -EINVAL;
}

int chaos::ds2482::set_read_ptr(uint8_t ptr) const
{
	struct chaos::ds2482_data *chip = dev_softc;
	uint8_t c[2];
	int r;

	c[0] = CMD_SET_READ_PTR;
	c[1] = ptr;
	r = dev_parent->write(this->addr(), c, 2);
	if (r < 0)
		return r;

	return 0;
}

static int ds2482_write_config(chaos::ds2482 *self, uint8_t config)
{
	uint8_t c[2];
	int r;

	c[0] = CMD_WRITE_CONFIG;
	c[1] = ~config << 4 | config;
	r = self->parent()->write(self->addr(), c, 2);
	if (r < 0)
		return r;

	return 0;
}

int chaos::ds2482::reset() const
{
	uint8_t c = CMD_1W_RESET;
	int r;

	r = parent()->write(this->addr(), &c, 1);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = parent()->read(this->addr(), &c, 1);
		if (r < 0)
			return r;
		if ((c & STATUS_1WB) == 0)
			break;
	}
	return 0;
}

// bit == 1 also means read time slot
static int ds2482_1w_single_bit(chaos::ds2482 *master, int bit)
{
	uint8_t c[2] = { CMD_1W_BIT, static_cast<uint8_t>(bit<<7) };
	int r;

	r = master->parent()->write(master->addr(), c, 2);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = master->parent()->read(master->addr(), c, 1);
		if (r < 0)
			return r;
		if ((c[0] & STATUS_1WB) == 0)
			break;
	}
	return !!(c[0] & STATUS_SBR);
}

int chaos::ds2482::write(uint8_t data) const
{
	uint8_t c[2] = { CMD_1W_WRITE, data };
	int r;

	r = parent()->write(this->addr(), c, 2);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = parent()->read(this->addr(), c, 1);
		if (r < 0)
			return r;
		if ((c[0] & STATUS_1WB) == 0)
			break;
	}
	return 0;
}

int chaos::ds2482::read() const
{
	uint8_t c = CMD_1W_READ;
	int r;

	r = parent()->write(addr(), &c, 1);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = parent()->read(this->addr(), &c, 1);
		if (r < 0)
			return r;
		if ((c & STATUS_1WB) == 0)
			break;
	}

	r = set_read_ptr(PTR_READ_DATA);
	if (r < 0)
		return r;

	r = parent()->read(addr(), &c, 1);
	if (r < 0)
		return r;

	return c;
}

/* dir - 0/1 direction to take in case of device conflict */
int chaos::ds2482::triplet(int dir) const
{
	uint8_t c[2] = { CMD_1W_TRIPLET, static_cast<uint8_t>(dir<<7) };
	int r;

	r = parent()->write(addr(), c, 2);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = parent()->read(addr(), c, 1);
		if (r < 0)
			return r;
		if ((c[0] & STATUS_1WB) == 0)
			break;
	}

	/* return value:
	 * bit 0: path taken
	 * bit 1: conflict?
	 */
	int bit1 = !!(c[0] & STATUS_SBR);
	int bit2 = !!(c[0] & STATUS_TSB);

	/* error, no device pulled the bit low */
	if (bit1 == 1 && bit2 == 1)
		return -ENODEV;

	dir = !!(c[0] & STATUS_DIR);

	/* conflict */
	if (bit1 == 0 && bit2 == 0)
		return 2 | dir;
	return dir;
}

int chaos::ds2482::init() const
{
	//struct chaos::ds2482_data *chip = master->dev_softc;
	int r = reset();
	if (r < 0)
		return r;
	return 0;
}
