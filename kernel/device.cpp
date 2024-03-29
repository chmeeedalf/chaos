/*
 * Copyright (c) 2014	Justin Hibbits
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

#include <chaos/device.h>

#include <stdio.h>
#include <string.h>
#include <util/shell.h>

namespace chaos {

static list<device> devices;

// root_bus is a sentinel.  We want to be able to use nullptr checks, so make
// the root_bus a different sentinel.
const device *root_bus = reinterpret_cast<device *>(-1);

void
device::attach(void)
{
	devices.add_tail(this);
}

device::~device()
{
	devices.remove(this);
}

static int
list_devices(void)
{
	device *d = devices.head();
	for (; d != nullptr; d = d->next())
		iprintf("%s\n\r", d->name());
	return (0);
}

const device *
device::find_device(const char *name)
{
	for (auto d = devices.head(); d != nullptr; d = d->next())
		if (strcmp(name, d->name()) == 0)
			return d;
	return nullptr;
}

static int
device_show(const char *devname)
{
	auto dev = device::find_device(devname);

	if (dev == nullptr) {
		iprintf("No such device: %s\r\n", devname);
		return (1);
	}

	iprintf("Name:\t%s\n\r", dev->name());
	dev->show();
	return (0);
}

CMD_FAMILY(device);
CMD(device, list, list_devices);
CMD(device, show, device_show);
}
