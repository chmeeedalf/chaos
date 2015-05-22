#include <stdbool.h>
#include <stdint.h>

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

#define LCD_I2C_PORT	I2C0_BASE
#define LCD_I2C_ADDR	0x3F

#define RS	0x01
#define RW	0x02
#define ENABLE	0x04

static int _backlight = 0x08;

static void send_byte(uint8_t addr, uint8_t byte, uint8_t mode);
static void write4bits(uint8_t addr, uint8_t bits);

static void i2c_write(uint8_t addr, uint8_t bits)
{
	MAP_I2CMasterSlaveAddrSet(LCD_I2C_PORT, addr, false);
	MAP_I2CMasterDataPut(LCD_I2C_PORT, bits | _backlight);
	MAP_I2CMasterControl(LCD_I2C_PORT, I2C_MASTER_CMD_SINGLE_SEND);
	while (MAP_I2CMasterBusy(LCD_I2C_PORT))
		;
}

void lcd_init(uint8_t addr)
{
#if 0
	i2c_write(addr, 0x30);
	MAP_SysCtlDelay(40 * 5000);
	i2c_write(addr, 0x30);
	MAP_SysCtlDelay(200 * 40);
	i2c_write(addr, 0x20);
	MAP_SysCtlDelay(40 * 5000);

	send_byte(addr, 0x28, 0);
	send_byte(addr, 0x08, 0);
	send_byte(addr, 0x01, 0);
	send_byte(addr, 0x06, 0);
	send_byte(addr, 0x0C, 0);
#else
	MAP_SysCtlDelay(40 * 50000);
	write4bits(addr, 0x03);
	MAP_SysCtlDelay(40 * 4500);
	write4bits(addr, 0x3);
	MAP_SysCtlDelay(40 * 4500);
	write4bits(addr, 0x3);
	MAP_SysCtlDelay(40 * 4500);
	i2c_write(addr, 0x20);
	MAP_SysCtlDelay(40 * 5000);

	send_byte(addr, 0x28, 0);
	send_byte(addr, 0x08, 0);
	send_byte(addr, 0x01, 0);
	send_byte(addr, 0x06, 0);
	send_byte(addr, 0x0F, 0);
#endif
}

static void pulse_enable(uint8_t addr, uint8_t bits)
{
	i2c_write(addr, bits | ENABLE);
	MAP_SysCtlDelay(40);
	i2c_write(addr, bits & ~ENABLE);
	MAP_SysCtlDelay(2000);
}

static void write4bits(uint8_t addr, uint8_t bits)
{
	i2c_write(addr, bits);
	pulse_enable(addr, bits);
}

static void send_byte(uint8_t addr, uint8_t byte, uint8_t mode)
{
	uint8_t hi = (byte & 0xf0);
	uint8_t lo = (byte & 0x0f) << 4;

	write4bits(addr, hi | mode);
	write4bits(addr, lo | mode);
}

int write_char(int ch)
{
	send_byte(LCD_I2C_ADDR, ch, RS);
	return 0;
}
