#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gpio.h>

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

static struct tiva_uart_gpio_config valid_configs[] = {
	{ .tx = {GPIO_PA1},
		.rx = {GPIO_PA0},
		.cts = {GPIO_PH1, GPIO_PM4, GPIO_PB4},
		.rts = {GPIO_PH0, GPIO_PB5},
		.dcd = {GPIO_PH2, GPIO_PM5, GPIO_PP3},
		.dsr = {GPIO_PH3, GPIO_PM6, GPIO_PP4},
		.dtr = {GPIO_PP2},
		.ri = {GPIO_PK7, GPIO_PM7}},
	{ .tx = {GPIO_PB1},
		.rx = {GPIO_PB0, GPIO_PQ4},
		.cts = {GPIO_PP3, GPIO_PN1},
		.rts = {GPIO_PE0, GPIO_PN0},
		.dcd = {GPIO_PE2, GPIO_PN2},
		.dsr = {GPIO_PE1, GPIO_PN3},
		.dtr = {GPIO_PE3, GPIO_PN4},
		.ri = {GPIO_PN5, GPIO_PE4}},
	{ .tx = {GPIO_PA7, GPIO_PD5},
		.rx = {GPIO_PA6, GPIO_PD4},
		.cts = {GPIO_PN3, GPIO_PD7},
		.rts = {GPIO_PN2, GPIO_PD6}},
	{ .tx = {GPIO_PA5, GPIO_PJ1},
		.rx = {GPIO_PA4, GPIO_PJ0},
		.cts = {GPIO_PP3, GPIO_PN5},
		.rts = {GPIO_PP4, GPIO_PN4}},
	{ .tx = {GPIO_PK1, GPIO_PA3},
		.rx = {GPIO_PK0, GPIO_PA2},
		.cts = {GPIO_PK3},
		.rts = {GPIO_PK2}},
	{ .tx = {GPIO_PC7},
		.rx = {GPIO_PC6}},
	{ .tx = {GPIO_PP1},
		.rx = {GPIO_PP0}},
	{ .tx = {GPIO_PC5},
		.rx = {GPIO_PC4}}
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
