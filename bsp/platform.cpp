#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <chaos/kernel.h>
#include "inc/hw_sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include <util/shell.h>

#define SYSCTL_DELAY_TICKS	3

static uint32_t sys_clk;

namespace bsp {
void set_systick(int hz)
{
	MAP_SysTickPeriodSet(sys_clk/hz);
}

void systick_enable()
{
	MAP_SysTickIntEnable();
	MAP_SysTickEnable();
}

void systick_disable()
{
	MAP_SysTickDisable();
}

void init(void)
{
	set_freq(configCPU_CLOCK_HZ);
}

}

void
set_freq(uint32_t mhz)
{
	sys_clk = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), mhz);
}

uint32_t
get_freq()
{
	return sys_clk;
}

void
udelay(uint32_t delay)
{
	MAP_SysCtlDelay(delay * ((sys_clk + (SYSCTL_DELAY_TICKS * 1000 - 3)) / SYSCTL_DELAY_TICKS / 1000) / 1000);
}

static int
cmd_peek(int addr)
{
	iprintf("%x: %x\n\r", addr, *(volatile int *)addr);
	return 0;
}

static int
cmd_poke(int addr, int val)
{
	*(volatile int *)addr = val;
	return 0;
}
CMD_FAMILY(memory);
CMD(memory, peek, cmd_peek);
CMD(memory, poke, cmd_poke);
