
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
#include <inc/hw_sysctl.h>

namespace tiva {

#define GPIO_TO_BIT(mmio) \
	((((uintptr_t)mmio >> 12) & 0xff) - 0x58)
class gpio_int : public gpio {
	using gpio_dir = chaos::gpio::gpio_pin::gpio_dir;
	using gpio_pull_up_down = chaos::gpio::gpio_pin::gpio_pull_up_down;
	using gpio_trigger = chaos::gpio::gpio_pin::gpio_trigger;
	using gpio_edge = chaos::gpio::gpio_pin::gpio_edge;
	public:
	struct gpio_mmio {
		uint32_t	gpio_data[0x100];
		uint32_t	gpio_dir;
		uint32_t	gpio_is;
		uint32_t	gpio_ibe;
		uint32_t	gpio_iev;
		uint32_t	gpio_im;
		uint32_t	gpio_ris;
		uint32_t	gpio_mis;
		uint32_t	gpio_icr;
		uint32_t	gpio_afsel;
		uint32_t	rsvd_2[(0x500 - 0x424) / 4];
		uint32_t	gpio_dr2r;
		uint32_t	gpio_dr4r;
		uint32_t	gpio_dr8r;
		uint32_t	gpio_odr;
		uint32_t	gpio_pur;
		uint32_t	gpio_pdr;
		uint32_t	gpio_slr;
		uint32_t	gpio_den;
		uint32_t	gpio_lock;
		uint32_t	gpio_cr;
		uint32_t	gpio_amsel;
		uint32_t	gpio_pctl;
		uint32_t	gpio_adcctl;
		uint32_t	gpio_dmactl;
		uint32_t	gpio_si;
		uint32_t	gpio_dr12a;
		uint32_t	gpio_wakepen;
		uint32_t	gpio_wakelvl;
		uint32_t	gpio_wakestat;
		uint32_t	rsvd_3[(0xfc0 - 0x54c) / 4];
	};
private:
	gpio_mmio *mmio;

protected:
	virtual void set_direction(chaos::gpio::gpio_pin *p,
			gpio_dir d);
	virtual void set_output(chaos::gpio::gpio_pin *p,
			bool state);

public:
	gpio_int(const char *name, gpio_mmio *mmio) :
		gpio(name), mmio(mmio) { *(uint32_t *)SYSCTL_RCGCGPIO |= (1 << GPIO_TO_BIT(mmio));}
	virtual bool state(chaos::gpio::gpio_pin *p);
	virtual void configure(chaos::gpio::gpio_pin *p);
	void init_pin(gpio_pin *p);
};

bool
gpio_int::state(chaos::gpio::gpio_pin *p)
{
	gpio_pin *tp = static_cast<gpio_pin*>(p);
	return !!(mmio->gpio_data[1 << tp->pin_no]);
}

#define PIN_MASK(p)	(1 << p)
void
gpio_int::configure(chaos::gpio::gpio_pin *p)
{
	uint32_t pm = PIN_MASK(static_cast<gpio_pin *>(p)->pin_no);
	mmio->gpio_dir = (mmio->gpio_dir & ~pm) |
		(p->direction == gpio_dir::OUTPUT ? pm : 0);
	mmio->gpio_odr = (mmio->gpio_odr & ~pm) |
		(p->open_drain ? pm : 0);
	mmio->gpio_pur = (mmio->gpio_dir & ~pm) |
		(p->updown == gpio_pull_up_down::PULLUP ? pm : 0);
	mmio->gpio_den |= pm;
}

void
gpio_int::init_pin(gpio_pin *p)
{
	uint32_t pmask = PIN_MASK(p->pin_no);
	p->direction = (mmio->gpio_dir & pmask) ?
		gpio_dir::OUTPUT :
		gpio_dir::INPUT;

	p->updown = (mmio->gpio_pur & pmask) ?  gpio_pull_up_down::PULLUP :
		((mmio->gpio_pdr & pmask) ?  gpio_pull_up_down::PULLDOWN :
		 gpio_pull_up_down::NONE);
	p->trigger = (mmio->gpio_is & pmask) ?  gpio_trigger::LEVEL :
		gpio_trigger::EDGE;
	p->edge = (mmio->gpio_ibe & pmask) ?  gpio_edge::BOTH :
		((mmio->gpio_iev & pmask) ?  gpio_edge::HIGH :
		 gpio_edge::LOW);

	p->open_drain = !!(mmio->gpio_odr & pmask);
}

void
gpio_int::set_direction(chaos::gpio::gpio_pin *p,
		chaos::gpio::gpio_pin::gpio_dir d)
{
	uint32_t pm = PIN_MASK(static_cast<gpio_pin*>(p)->pin_no);
	mmio->gpio_dir &= ~pm;
	
	if (d == chaos::gpio::gpio_pin::gpio_dir::OUTPUT)
		mmio->gpio_dir |= pm;
	p->direction = d;
}

void
gpio_int::set_output(chaos::gpio::gpio_pin *p, bool s)
{
	uint32_t pm = PIN_MASK(static_cast<gpio_pin*>(p)->pin_no);
	mmio->gpio_data[pm] = (s ? pm : 0);
}

void
gpio::gpio_pin::init_pin()
{
	static_cast<gpio_int *>(port)->init_pin(this);
}

#if 0
void
gpio_init(const char *pin, enum gpio_mode mode, int value)
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

int
gpio_set(pin, int value)
{
	value = !!value;

	if (valid_pin(pin))
		return (-1);

	MAP_GPIOPinWrite(TO_BASE(pin), TO_PIN(pin), (value << TO_PIN(pin)));
}

int
gpio_get(const char *pin)
{
	if (validate_pin(pin))
		return (-1);
	return MAP_GPIOPinRead(TO_BASE(pin), TO_PIN(pin));
}

static int
gpio_configure_pin(const char *pin, int mode, int value)
{
	gpio_init(pin, (enum gpio_mode)mode, value);
	return (0);
}

static int
gpio_cli_get(const char *pin)
{
	int res = gpio_get(pin);

	if (res == -1)
		return (-1);
	iprintf("%d\n", res);

	return (0);
}

static int
gpio_cli_set(const char *pin, int value)
{
	gpio_set(pin, value);
	return (0);
}

CMD_FAMILY(gpio);
CMD(gpio, configure, gpio_configure_pin);
CMD(gpio, set, gpio_cli_set);
CMD(gpio, get, gpio_cli_get);
#endif

DEVICE_TYPE(gpio, gpio_int, gpio_PA, ("PA", (gpio_int::gpio_mmio *)GPIO_PORTA_AHB_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PB, ("PB", (gpio_int::gpio_mmio *)GPIO_PORTB_AHB_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PC, ("PC", (gpio_int::gpio_mmio *)GPIO_PORTC_AHB_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PD, ("PD", (gpio_int::gpio_mmio *)GPIO_PORTD_AHB_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PE, ("PE", (gpio_int::gpio_mmio *)GPIO_PORTE_AHB_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PF, ("PF", (gpio_int::gpio_mmio *)GPIO_PORTF_AHB_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PG, ("PG", (gpio_int::gpio_mmio *)GPIO_PORTG_AHB_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PH, ("PH", (gpio_int::gpio_mmio *)GPIO_PORTH_AHB_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PJ, ("PJ", (gpio_int::gpio_mmio *)GPIO_PORTJ_AHB_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PK, ("PK", (gpio_int::gpio_mmio *)GPIO_PORTK_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PL, ("PL", (gpio_int::gpio_mmio *)GPIO_PORTL_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PM, ("PM", (gpio_int::gpio_mmio *)GPIO_PORTM_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PN, ("PN", (gpio_int::gpio_mmio *)GPIO_PORTN_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PP, ("PP", (gpio_int::gpio_mmio *)GPIO_PORTP_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PQ, ("PQ", (gpio_int::gpio_mmio *)GPIO_PORTQ_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PR, ("PR", (gpio_int::gpio_mmio *)GPIO_PORTR_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PS, ("PS", (gpio_int::gpio_mmio *)GPIO_PORTS_BASE));
DEVICE_TYPE(gpio, gpio_int, gpio_PT, ("PT", (gpio_int::gpio_mmio *)GPIO_PORTT_BASE));

}
