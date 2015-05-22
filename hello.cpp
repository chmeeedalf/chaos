#include <reent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include <drivers/1w.h>
#include <drivers/bitbang_1w.h>
#include <drivers/tiva_serial.h>
#endif

#include <sys/device.h>
#include <sys/kernel.h>
#include <sys/thread.h>

#include <mach/gpio.h>

#if 0
#include <drivers/hd44780.h>
#include <drivers/ds18b20_1w.h>
#endif

#include <util/shell.h>
#include <tiva/serial.h>

#include "pinout.h"

#if 0
DEVICE(bitbang_1w, sensb1, "Sensors bus 1", ROOT_BUS, &bitbang_1w_methods, .pin = GPIO_PC7);
DEVICE(ds18b20, sens1, "Sensor 1", &sensb1, &ds18b20_1w_methods);
//DEVICE(hd44780_gpio_bus, lcd_bus, "LCD Bus", ROOT_BUS, &hd44780_gpio_bus_methods);
//DEVICE(hd44780, lcd, "LCD", &lcd_bus, &hd44780_methods, .Te = 10);

THREAD("Test thread", NULL, 1024, 1024, 1);
#endif
DEVICE(tiva::serial, uart, "UART0", chaos::root_bus, .baudrate=9600);

static int tiva_read(void *ts, char *buf, int n)
{
	return reinterpret_cast<tiva::serial*>(ts)->cdev_read(buf, n);
}

static int tiva_write(void *ts, const char *buf, int n)
{
	return reinterpret_cast<tiva::serial*>(ts)->cdev_write(buf, n);
}

FILE *stdin_ptr = nullptr;

int
main(int argc, char **argv)
{
	int gpio = 0x01;
	int temp;
	char buf[120];

	PinoutSet(true, true);
	set_freq(120 * MHZ);

	chaos::initialize_devices();
#if 0
	w1_read_rom(&sensb1, &sens_addr);
	//BUS_METHOD(hd44780, print, &lcd, "hello");
#endif
	stdin_ptr = (FILE *)-1;
	asm volatile ("" ::: "memory");
	stdin = funopen(uart, tiva_read, nullptr, nullptr, nullptr);
	stdout = funopen(uart, nullptr, tiva_write, nullptr, nullptr);
	stdin_ptr = stdin;
	while (1) {
#if 0
		MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, (gpio = gpio ^ 0x03));
#endif
		chaos::shell::shell("Beer test> ");
	}
}
