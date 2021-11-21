/*
 * Copyright (c) 2021	Justin Hibbits
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <errno.h>
#include <tiva/spi.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>

namespace tiva {
namespace {

	struct gpio_pin_id {
		uint32_t gpio_sysctl_periph;
		uint32_t gpio_base;
		uint32_t gpio_pin;
		uint32_t gpio_ssi_assign;
	};
	struct spi_hw_config {
		spi_bus::spi_id id;
		uint32_t ssi_base;
		uint32_t sysctl_periph;
		gpio_pin_id clock;
		gpio_pin_id fss;
		gpio_pin_id tx;
		gpio_pin_id rx;
	};

	// XXX: These configurations are only for the TM4C1294NCPDT
	spi_hw_config spis[] = {
		{ spi_bus::spi_id::SPI0, SSI0_BASE, SYSCTL_PERIPH_SSI0,
			{ SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PA2_SSI0CLK },
			{ SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PA3_SSI0FSS },
			{ SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_PA4_SSI0XDAT0 },
			{ SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PA5_SSI0XDAT1 },
		},
		{ spi_bus::spi_id::SPI1, SSI1_BASE, SYSCTL_PERIPH_SSI1,
			{ SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_PB5_SSI1CLK },
			{ SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PB4_SSI1FSS },
			{ SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PE4_SSI1XDAT0 },
			{ SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PE5_SSI1XDAT1 },
		},
		{ spi_bus::spi_id::SPI2, SSI2_BASE, SYSCTL_PERIPH_SSI2,
			{ SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PD3_SSI2CLK },
			{ SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PD2_SSI2FSS },
			{ SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PD1_SSI2XDAT0 },
			{ SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PD0_SSI2XDAT1 },
		},
		{ spi_bus::spi_id::SPI3, SSI3_BASE, SYSCTL_PERIPH_SSI3,
			{ SYSCTL_PERIPH_GPIOQ, GPIO_PORTQ_BASE, GPIO_PIN_2, GPIO_PQ2_SSI3XDAT0 },
			{ SYSCTL_PERIPH_GPIOQ, GPIO_PORTQ_BASE, GPIO_PIN_3, GPIO_PQ3_SSI3XDAT1 },
			{ SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PF4_SSI3XDAT2 },
			{ SYSCTL_PERIPH_GPIOP, GPIO_PORTP_BASE, GPIO_PIN_1, GPIO_PP1_SSI3XDAT3 },
		},
	};
}
spi_bus::spi_bus(const char *name, spi_id id, uint32_t bitrate) :
	device(name, chaos::root_bus), chaos::spi_bus()
{
	spi = id;

	for (auto &cfg : spis) {
		if (cfg.id == id) {
			MAP_SysCtlPeripheralEnable(cfg.sysctl_periph);
			MAP_SysCtlPeripheralEnable(cfg.clock.gpio_sysctl_periph);
			MAP_SysCtlPeripheralEnable(cfg.fss.gpio_sysctl_periph);
			MAP_SysCtlPeripheralEnable(cfg.tx.gpio_sysctl_periph);
			MAP_SysCtlPeripheralEnable(cfg.rx.gpio_sysctl_periph);
			MAP_GPIOPinConfigure(cfg.clock.gpio_ssi_assign);
			MAP_GPIOPinConfigure(cfg.fss.gpio_ssi_assign);
			MAP_GPIOPinConfigure(cfg.tx.gpio_ssi_assign);
			MAP_GPIOPinConfigure(cfg.rx.gpio_ssi_assign);
			MAP_GPIOPinTypeSSI(cfg.clock.gpio_base, cfg.clock.gpio_pin);
			MAP_GPIOPinTypeSSI(cfg.fss.gpio_base, cfg.fss.gpio_pin);
			MAP_GPIOPinTypeSSI(cfg.tx.gpio_base, cfg.tx.gpio_pin);
			MAP_GPIOPinTypeSSI(cfg.rx.gpio_base, cfg.rx.gpio_pin);

			MAP_SSIConfigSetExpClk(cfg.ssi_base, get_freq(),
			    SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, bitrate, 8);
			MAP_SSIEnable(cfg.ssi_base);
			base = cfg.ssi_base;
			break;
		}
	}
}

void
spi_bus::transfer(chaos::spi_device *slave, chaos::spi_bus::spi_transfer *xfer)
{
	bool cs_asserted = false;
	gpio_pin_id *cs_pin;
	sem.take_lock();
	constexpr uint32_t SPI_XFER_NO_CS =
	    chaos::spi_bus::spi_transfer::SPI_XFER_NO_CS;

	if (slave->chipselect() != -1) {
		// TODO: Map chip select IDs to GPIOs
		// For now, chipselect *must* be -1
		if (slave->chipselect() != 0) {
			xfer->error = ENXIO;
			return;
		}
	}
	if (xfer->flags & SPI_XFER_NO_CS) {
		for (auto &cfg : spis) {
			if (cfg.id == spi) {
				cs_pin = &cfg.fss;
				// Turn chip select into GPIO, output hi
				MAP_GPIOPinTypeGPIOOutput(cs_pin->gpio_base,
				    cs_pin->gpio_pin);
			}
		}
	}
	cs_asserted = true;
	size_t i;
	for (i = 0; i < xfer->tx_len; i++) {
		uint32_t buf;
		MAP_SSIDataPut(base, xfer->tx_buffer[i]);
		MAP_SSIDataGet(base, &buf);
		if (i < xfer->rx_len)
			xfer->rx_buffer[i] = buf;
	}
	for (; i < xfer->rx_len; i++) {
		uint32_t buf;
		MAP_SSIDataPut(base, 0);
		MAP_SSIDataGet(base, &buf);
		xfer->rx_buffer[i] = buf;
	}

	if (xfer->flags & SPI_XFER_NO_CS) {
		// restore chip select to fss usage
		MAP_GPIOPinTypeSSI(cs_pin->gpio_base, cs_pin->gpio_ssi_assign);
	}
	if (cs_asserted) {
		// deassert
	}
	sem.release();
}
}
