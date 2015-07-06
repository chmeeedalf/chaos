/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <chaos/kernel.h>
#include <stdio.h>
#include <drivers/hd44780.h>
#include <gpio.h>

/* there are actually read commands/data too, but we won't use that.. for now... maybe BF is important */
#if 0
static int lcd_gpio_init(device bus, device parent)
{
	struct hd44780_gpio_bus_softc *sc = bus->dev_softc;

	gpio_init(sc->rs, GPIO_OUTPUT, 0);
	gpio_init(sc->rw, GPIO_OUTPUT, 0);
	gpio_init(sc->e, GPIO_OUTPUT, 0);
	gpio_init(sc->d4, GPIO_OUTPUT, 0);
	gpio_init(sc->d5, GPIO_OUTPUT, 0);
	gpio_init(sc->d6, GPIO_OUTPUT, 0);
	gpio_init(sc->d7, GPIO_OUTPUT, 0);

	return (0);
}
static void lcd_gpio_put(device lcd, int rs, int data, int delay)
{
	struct hd44780_gpio_bus_softc *softc = lcd->dev_softc;

	gpio_set(softc->rs, rs);
	gpio_set(softc->rw, 0);
	udelay(delay);
	gpio_set(softc->e, 1);
	udelay(delay);
	gpio_set(softc->d4, data & 1);
	gpio_set(softc->d5, data>>1 & 1);
	gpio_set(softc->d6, data>>2 & 1);
	gpio_set(softc->d7, data>>3 & 1);
	gpio_set(softc->e, 0);
}

int lcd_gpio_read(device lcd, int rs, int delay)
{
	struct hd44780_gpio_bus_softc *softc = lcd->dev_softc;
	int tmp;

	gpio_init(softc->d4, GPIO_INPUT, 0);
	gpio_init(softc->d5, GPIO_INPUT, 0);
	gpio_init(softc->d6, GPIO_INPUT, 0);
	gpio_init(softc->d7, GPIO_INPUT, 0);

	gpio_set(softc->rs, rs);
	gpio_set(softc->rw, 1);
	udelay(delay);
	gpio_set(softc->e, 1);
	udelay(delay);

	tmp = 0;
	tmp |= gpio_get(softc->d4) << 4;
	tmp |= gpio_get(softc->d5) << 5;
	tmp |= gpio_get(softc->d6) << 6;
	tmp |= gpio_get(softc->d7) << 7;
	gpio_set(softc->e, 0);

	udelay(delay);
	gpio_set(softc->e, 1);
	udelay(delay);

	tmp |= gpio_get(softc->d4) << 0;
	tmp |= gpio_get(softc->d5) << 1;
	tmp |= gpio_get(softc->d6) << 2;
	tmp |= gpio_get(softc->d7) << 3;
	gpio_set(softc->e, 0);

	gpio_init(softc->d4, GPIO_OUTPUT, 0);
	gpio_init(softc->d5, GPIO_OUTPUT, 0);
	gpio_init(softc->d6, GPIO_OUTPUT, 0);
	gpio_init(softc->d7, GPIO_OUTPUT, 0);

	return tmp;
}

const struct hd44780_bus_methods hd44780_gpio_bus_methods = {
	.init = lcd_gpio_init,
	.write = lcd_gpio_put,
	.read = lcd_gpio_read
};
#endif

static inline void lcd_put(const hd44780 *lcd, int rs, int data)
{

	lcd->write(rs, data);
}

static inline void lcd_cmd(hd44780 *lcd, int cmd)
{
	int timeout = 1000;

	while (--timeout) {
		/* bits 0-6 are address, that might be useful too */
		if ((lcd->read(0) & 0x80) == 0)
			break;
	}
#if 0
	if (timeout == 0)
		printf("%s, timeouted at cmd %x\n", __func__, cmd);
#endif

	lcd_put(lcd, 0, cmd>>4);
	lcd_put(lcd, 0, cmd&0xf);
}

static inline void lcd_data(const hd44780 *lcd, int cmd)
{
	int timeout = 1000;

	while (--timeout) {
		/* bits 0-6 are address, that might be useful too */
		if ((lcd->read(0) & 0x80) == 0)
			break;
	}
#if 0
	if (timeout == 0)
		printf("%s, timeouted at cmd %x\n", __func__, cmd);
#endif

	lcd_put(lcd, 1, cmd>>4);
	lcd_put(lcd, 1, cmd&0xf);
}

int hd44780::init_lcd() const
{
	int caps;

	caps = this->get_caps();
	/* reset sequence */
	lcd_put(this, 0, 3);
	udelay(4100);
	lcd_put(this, 0, 3);
	udelay(100);

	lcd_put(this, 0, 3);
	udelay(37);
	lcd_put(this, 0, 2);
	udelay(37);

	/* ok, in 4-bit mode now */
	int tmp = 0;
	if (caps & HD44780_CAPS_2LINES)
		tmp |= 1<<3;
	if (caps & HD44780_CAPS_5X10)
		tmp |= 1<<2;
	this->cmd(CMD_FUNCTION_SET | tmp);
	this->cmd(CMD_DISPLAY_ON_OFF); /* display, cursor and blink off */
	this->cmd(CMD_CLEAR);

	this->cmd(CMD_ENTRY_MODE | HD44780_ENTRY_INC);

	return (0);
}


void hd44780::clear() const
{
	this->cmd(CMD_CLEAR);
}

void hd44780::home() const
{
	this->cmd(CMD_HOME);
}

void hd44780::entry_mode(int mode) const
{
	this->cmd(CMD_ENTRY_MODE | (mode&0x3));
}

void hd44780::onoff(int features) const
{
	this->cmd(CMD_DISPLAY_ON_OFF | (features&0x7));
}

void hd44780::shift(int shift) const
{
	this->cmd(CMD_SHIFT | (shift&0xc));
}

void hd44780::set_position(int pos) const
{
	this->cmd(CMD_DDRAM_ADDR | (pos&0x7f));
}

void hd44780::print(const char *str) const
{
	while (*str)
		lcd_data(this, *str++);
}
