#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <drivers/gpio.h>

#include <inc/hw_memmap.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <tiva/serial.h>
#include <chaos/kernel.h>

#define MAX_GPIO 3
struct tiva_uart_gpio_config {
	int tx[MAX_GPIO];
	int rx[MAX_GPIO];
	int cts[MAX_GPIO];
	int rts[MAX_GPIO];
	int dcd[MAX_GPIO];
	int dsr[MAX_GPIO];
	int dtr[MAX_GPIO];
	int ri[MAX_GPIO];
};

int
tiva::serial::init() const
{
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0 + 0x1000 * uart);
	MAP_UARTEnable(UART0_BASE + (uart << 12));

	// TODO: Check pin configuration
	MAP_UARTConfigSetExpClk(UART0_BASE + 0x1000 * uart,
		get_freq(), baudrate,
		(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));

	return 0;
}

int
tiva::serial::cdev_read(char *buf, int len) const
{
	uint8_t *b = (uint8_t *)buf;

	for (int i = 0; i < len; i++)
		b[i] = MAP_UARTCharGet(UART0_BASE + 0x1000 * uart);

	return len;
}

int
tiva::serial::cdev_write(const char *buf, int len) const
{
	const uint8_t *b = (const uint8_t *)buf;

	for (int i = 0; i < len; i++)
		MAP_UARTCharPut(UART0_BASE + 0x1000 * uart, b[i]);

	return len;
}

int
tiva::serial::destroy() const
{
	return 0;
}

int
tiva::serial::probe() const
{
	return 0;
}

int tiva::serial::show() const
{
	iprintf("Baud:\t%d\n\r", this->baudrate);

	return 0;
}
