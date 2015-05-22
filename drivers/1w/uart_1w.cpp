#include <sys/kernel.h>
#include <drivers/uart_1w.h>
#include <drivers/1w.h>
#include <gpio.h>
#include <lock.h>
#include <errno.h>

static int uart_1w_reset(uart_1w *master)
{
	device_t parent = device_get_parent(master);
	struct uart_1w_softc *chip = master->dev_softc;
	int pin = chip->pin;
	uint8_t read_char;

	BUS_METHOD(tiva_serial, configure, parent, 9600, 8, 0, 1);
	BUS_METHOD(tiva_serial, write, parent, 0xF0);
	BUS_METHOD(tiva_serial, read, parent, &read_char, 1);

	return 0;
}

static int uart_1w_bit(uart_1w *master, uint8_t bit)
{
	device_t parent = device_get_parent(master);

	bit = bit ? 0xFF : 0;
	BUS_METHOD(tiva_serial, write, parent, &bit, 1);
	BUS_METHOD(tiva_serial, read, parent, &bit, 1);

	return bit;
}

static int uart_1w_write(uart_1w *master, uint8_t data)
{
	struct uart_1w_softc *chip = master->dev_softc;
	int pin = chip->pin;
	int i;
	struct lock l;

	lock(&l);
	for (i=0; i<8; i++) {
		uart_1w_bit(master, data & 0x1);
		data >>= 1;
	}
	unlock(&l);

	return 0;
}

static int uart_1w_read(uart_1w *master)
{
	struct uart_1w_softc *chip = master->dev_softc;
	int pin = chip->pin;
	uint8_t data = 0;
	uint8_t bit;
	int i;
	struct lock l;

	lock(&l);
	for (i=0; i<8; i++) {
		BUS_METHOD(tiva_serial, write, parent, &(uint8_t){0}, 1);
		data |= (BUS_METHOD(tiva_serial, read, parent, &bit, 1) & 0x1) << i;
	}
	unlock(&l);

	return data;
}

/* dir - 0/1 direction to take in case of device conflict */
static int uart_1w_triplet(uart_1w *master, int dir)
{
	struct uart_1w_softc *chip = master->dev_softc;
	int pin = chip->pin;
	struct lock l;

	lock(&l);
	int bit1 = uart_1w_bit(pin, 1);
	int bit2 = uart_1w_bit(pin, 1);

	if (bit1 ^ bit2)
		uart_1w_bit(pin, bit1);
	else
		uart_1w_bit(pin, dir);
	unlock(&l);

	/* return value:
	 * bit 0: path taken
	 * bit 1: conflict?
	 */

	/* error, no device pulled the bit low */
	if (bit1 == 1 && bit2 == 1)
		return -ENODEV;

	/* conflict */
	if (bit1 == 0 && bit2 == 0)
		return 2 | dir;
	return bit1;
}

int uart_1w_init(device_t master, device_t parent __unused)
{
	struct uart_1w_softc *chip = master->dev_softc;
	int pin = chip->pin;

	gpio_init(pin, GPIO_INPUT, 0);

	return 0;
}

const struct w1_methods uart_1w_methods = {
	.init = uart_1w_init,
	.w1_reset = uart_1w_reset,
	.w1_triplet = uart_1w_triplet,
	.w1_write = uart_1w_write,
	.w1_read = uart_1w_read,
};

