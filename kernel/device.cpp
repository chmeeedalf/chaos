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

extern const chaos::device<void,const chaos::device<void,void>> *sys_devices, *sys_devices_end;
namespace chaos {

int
initialize_device(const chaos::device<void,const chaos::device<void,void>> *dev)
{
	if (dev->parent() != nullptr) {
		initialize_device(reinterpret_cast<decltype(dev)>(dev->parent()));
	}

	if (dev->dev_run->state == chaos::device_state::DEVICE_PREINIT) {
		dev->init();
		dev->dev_run->state = chaos::device_state::DEVICE_INITIALIZE;
	}
	return 0;
}

int
initialize_devices(void)
{
	const chaos::device<void,const chaos::device<void,void>> **d = &sys_devices;

	for (; d < &sys_devices_end; d++) {
		initialize_device(*d);
	}
	return 0;
}

static int
list_devices(void)
{
	auto d = &sys_devices;

	for (; d < &sys_devices_end; d++) {
		iprintf("%s\t%s\n\r", (*d)->name(), (*d)->descr());
	}
	return 0;
}

const chaos::device<void,const chaos::device<void,void>> *
find_device(const char *name)
{
	for (auto d = &sys_devices; d < &sys_devices_end; d++) {
		if (strcmp(name, (*d)->name()) == 0)
			return *d;
	}
	return nullptr;
}

static int
device_show(const char *devname)
{
	auto dev = find_device(devname);

	if (dev == nullptr) {
		iprintf("No such device: %s\r\n", devname);
		return (1);
	}

	iprintf("Name:\t%s\n\r", dev->name());
	iprintf("Description:\t%s\n\r", dev->descr());
	dev->show();
	return (0);
}

CMD_FAMILY(device);
CMD(device, list, list_devices);
CMD(device, show, device_show);
}
