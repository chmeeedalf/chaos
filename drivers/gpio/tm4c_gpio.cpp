
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

namespace tiva {

class gpio_int : public gpio {
	public:
	struct gpio_mmio {
		uint32_t	gpio_data;
		uint32_t	rsvd_1[0x3fc / 4];
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
		uint32_t	gpio_dr24;
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
			chaos::gpio::gpio_pin::gpio_dir d);
	virtual void set_output(chaos::gpio::gpio_pin *p,
			bool state);

public:
	gpio_int(const char *name, gpio_mmio *mmio) :
		gpio(name), mmio(mmio) {}
	virtual bool state(chaos::gpio::gpio_pin *p);
	virtual void configure(chaos::gpio::gpio_pin *p);
	void init_pin(gpio_pin *p);
};

bool
gpio_int::state(chaos::gpio::gpio_pin *p)
{
	gpio_pin *tp = static_cast<gpio_pin*>(p);
	return !!(mmio->gpio_data >> tp->pin_no);
}

void
gpio_int::configure(chaos::gpio::gpio_pin *p)
{
}

#define PIN_MASK(p)	(1 << p)
void
gpio_int::init_pin(gpio_pin *p)
{
	uint32_t pmask = PIN_MASK(p->pin_no);
	p->direction = (mmio->gpio_dir & pmask) ?
		chaos::gpio::gpio_pin::gpio_dir::OUTPUT :
		chaos::gpio::gpio_pin::gpio_dir::INPUT;

	p->updown = (mmio->gpio_pur & pmask) ?
		chaos::gpio::gpio_pin::gpio_pull_up_down::PULLUP :
		((mmio->gpio_pdr & pmask) ?
		 chaos::gpio::gpio_pin::gpio_pull_up_down::PULLDOWN :
		 chaos::gpio::gpio_pin::gpio_pull_up_down::NONE);
	p->trigger = (mmio->gpio_is & pmask) ?
		chaos::gpio::gpio_pin::gpio_trigger::LEVEL :
		chaos::gpio::gpio_pin::gpio_trigger::EDGE;
	p->edge = (mmio->gpio_ibe & pmask) ?
		chaos::gpio::gpio_pin::gpio_edge::BOTH :
		((mmio->gpio_iev & pmask) ?
		 chaos::gpio::gpio_pin::gpio_edge::HIGH :
		 chaos::gpio::gpio_pin::gpio_edge::LOW);

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
	if (s)
		mmio->gpio_data |= pm;
	else
		mmio->gpio_data &= ~pm;
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

gpio_int gpio_PA_int("PA", (gpio_int::gpio_mmio *)GPIO_PORTA_AHB_BASE);
gpio_int gpio_PB_int("PB", (gpio_int::gpio_mmio *)GPIO_PORTB_AHB_BASE);
gpio_int gpio_PC_int("PC", (gpio_int::gpio_mmio *)GPIO_PORTC_AHB_BASE);
gpio_int gpio_PD_int("PD", (gpio_int::gpio_mmio *)GPIO_PORTD_AHB_BASE);
gpio_int gpio_PE_int("PE", (gpio_int::gpio_mmio *)GPIO_PORTE_AHB_BASE);
gpio_int gpio_PF_int("PF", (gpio_int::gpio_mmio *)GPIO_PORTF_AHB_BASE);
gpio_int gpio_PG_int("PG", (gpio_int::gpio_mmio *)GPIO_PORTG_AHB_BASE);
gpio_int gpio_PH_int("PH", (gpio_int::gpio_mmio *)GPIO_PORTH_AHB_BASE);
gpio_int gpio_PJ_int("PJ", (gpio_int::gpio_mmio *)GPIO_PORTJ_AHB_BASE);
gpio_int gpio_PK_int("PK", (gpio_int::gpio_mmio *)GPIO_PORTK_BASE);
gpio_int gpio_PL_int("PL", (gpio_int::gpio_mmio *)GPIO_PORTL_BASE);
gpio_int gpio_PM_int("PM", (gpio_int::gpio_mmio *)GPIO_PORTM_BASE);
gpio_int gpio_PN_int("PN", (gpio_int::gpio_mmio *)GPIO_PORTN_BASE);
gpio_int gpio_PP_int("PP", (gpio_int::gpio_mmio *)GPIO_PORTP_BASE);
gpio_int gpio_PQ_int("PQ", (gpio_int::gpio_mmio *)GPIO_PORTQ_BASE);
gpio_int gpio_PR_int("PR", (gpio_int::gpio_mmio *)GPIO_PORTR_BASE);
gpio_int gpio_PS_int("PS", (gpio_int::gpio_mmio *)GPIO_PORTS_BASE);
gpio_int gpio_PT_int("PT", (gpio_int::gpio_mmio *)GPIO_PORTT_BASE);

gpio *gpio_PA = &gpio_PA_int;
gpio *gpio_PB = &gpio_PB_int;
gpio *gpio_PC = &gpio_PC_int;
gpio *gpio_PD = &gpio_PD_int;
gpio *gpio_PE = &gpio_PE_int;
gpio *gpio_PF = &gpio_PF_int;
gpio *gpio_PG = &gpio_PG_int;
gpio *gpio_PH = &gpio_PH_int;
gpio *gpio_PJ = &gpio_PJ_int;
gpio *gpio_PK = &gpio_PK_int;
gpio *gpio_PL = &gpio_PL_int;
gpio *gpio_PM = &gpio_PM_int;
gpio *gpio_PN = &gpio_PN_int;
gpio *gpio_PP = &gpio_PP_int;
gpio *gpio_PQ = &gpio_PQ_int;
gpio *gpio_PR = &gpio_PR_int;
gpio *gpio_PS = &gpio_PS_int;
gpio *gpio_PT = &gpio_PT_int;
}
