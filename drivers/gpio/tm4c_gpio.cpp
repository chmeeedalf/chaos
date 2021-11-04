
#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <mach/gpio.h>
#include <stdio.h>
#include <util/shell.h>
#include <chaos/device.h>
#include <tiva/gpio.h>

#define TO_BASE(pin)	(GPIO_PORTA_AHB_BASE + (((pin & 0xF0) - 0x10) << 8))
#define TO_PIN(pin)		(1 << (pin & 0x0F))

void
gpio_init(int pin, enum gpio_mode mode, int value)
{
	uint32_t base = TO_BASE(pin);
	pin = 1 << (pin & 0x0F);

	switch (mode) {
		case GPIO_OUTPUT:
			MAP_GPIOPinTypeGPIOOutput(base, pin);
			gpio_set(pin, value);
			break;
		case GPIO_OUTPUT_SLOW:
			MAP_GPIOPinTypeGPIOOutput(base, pin);
			MAP_GPIOPadConfigSet(base, pin, GPIO_STRENGTH_8MA_SC,
			    GPIO_PIN_TYPE_STD);
			gpio_set(pin, value);
			break;
		case GPIO_INPUT:
			MAP_GPIOPinTypeGPIOInput(base, pin);
			break;
		case GPIO_INPUT_PD:
			MAP_GPIOPinTypeGPIOInput(base, pin);
			MAP_GPIOPadConfigSet(base, pin, GPIO_STRENGTH_8MA,
			    GPIO_PIN_TYPE_STD_WPD);
			break;
		case GPIO_INPUT_PU:
			MAP_GPIOPinTypeGPIOInput(base, pin);
			MAP_GPIOPadConfigSet(base, pin, GPIO_STRENGTH_8MA,
			    GPIO_PIN_TYPE_STD_WPU);
			break;
		case GPIO_ANALOG:
			MAP_GPIOPinTypeGPIOInput(base, pin);
			MAP_GPIOPadConfigSet(base, pin, GPIO_STRENGTH_8MA,
			    GPIO_PIN_TYPE_ANALOG);
			break;
	}
}

void
gpio_set(int pin, int value)
{
	value = !!value;

	MAP_GPIOPinWrite(TO_BASE(pin), TO_PIN(pin), (value << TO_PIN(pin)));
}

int
gpio_get(int pin)
{
	return MAP_GPIOPinRead(TO_BASE(pin), TO_PIN(pin));
}

static int
gpio_configure_pin(int pin, int mode, int value)
{
	gpio_init(pin, (enum gpio_mode)mode, value);
	return (0);
}

static int
gpio_cli_get(int pin)
{
	iprintf("%d\n", gpio_get(pin));
	return (0);
}

static int
gpio_cli_set(int pin, int value)
{
	gpio_set(pin, value);
	return (0);
}

CMD_FAMILY(gpio);
CMD(gpio, configure, gpio_configure_pin);
CMD(gpio, set, gpio_cli_set);
CMD(gpio, get, gpio_cli_get);
