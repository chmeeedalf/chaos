#include <tiva/i2c.h>
#include <inc/hw_memmap.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>

namespace tiva {

static const uint32_t i2c_base[] = {
	[i2c_bus_softc::I2C0] = I2C0_BASE,
	[i2c_bus_softc::I2C1] = I2C1_BASE,
	[i2c_bus_softc::I2C2] = I2C2_BASE,
	[i2c_bus_softc::I2C3] = I2C3_BASE,
	[i2c_bus_softc::I2C4] = I2C4_BASE,
	[i2c_bus_softc::I2C5] = I2C5_BASE,
	[i2c_bus_softc::I2C6] = I2C6_BASE,
	[i2c_bus_softc::I2C7] = I2C7_BASE,
	[i2c_bus_softc::I2C8] = I2C8_BASE,
	[i2c_bus_softc::I2C9] = I2C9_BASE
};

int i2c_bus::init(void) const
{
	MAP_I2CMasterEnable(i2c_base[this->dev_softc->bus]);
	return 0;
}

void i2c_bus::i2c_start(uint8_t addr, bool recv) const
{
	MAP_I2CMasterSlaveAddrSet(i2c_base[this->dev_softc->bus], addr, recv);
}

void i2c_bus::i2c_restart() const
{
}

void i2c_bus::i2c_stop() const
{
}

uint8_t i2c_bus::i2c_read_byte() const
{
	return MAP_I2CMasterDataGet(i2c_base[this->dev_softc->bus]);
}

void i2c_bus::i2c_write_byte(uint8_t byte) const
{
	MAP_I2CMasterDataPut(i2c_base[this->dev_softc->bus], byte);
}

int i2c_bus::i2c_read(uint8_t addr, uint8_t *data, int len) const
{
	i2c_start(addr, true);

	if (len == 1) {
		*data = i2c_read_byte();
	} else {
	}

	return 0;
}

int i2c_bus::i2c_write(uint8_t addr, uint8_t *data, int len) const
{
	i2c_start(addr, false);

	if (len == 1) {
	} else {
	}

	return 0;
}

}
