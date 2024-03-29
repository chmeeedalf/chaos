/*
 * Copyright (c) 2015	Justin Hibbits
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

#include <sys/cdefs.h>
#include <sys/types.h>
#include <chaos/kernel.h>
#include <app/user_config.h>
#include <tuple>

/* C Preprocessor abuse */
//#include <chaos/cpp.h>
#include <chaos/list.h>

#ifndef SYS_DEVICE_H
#define SYS_DEVICE_H

namespace chaos {

class onewire_bus;
class i2c_bus;
class spi_bus;

/*
 * All the virtual methods are const qualified, because they act on the mutable
 * data in the softc.
 */

template <typename BusType>
class bus_base {
public:
	virtual ~bus_base() = default;
	virtual const BusType *as_bus(std::type_identity<BusType>) const { return nullptr; }

};

template <typename ... bus_list>
class bus_mid : public bus_base<bus_list>...
{
	public:
		using bus_base <bus_list>::as_bus...;
};

template <typename UserBuses, typename DefaultBuses>
class device_core;

template <typename ... user_buses, typename ... default_buses>
class device_core<std::tuple<user_buses...>, std::tuple<default_buses...>> :
    public bus_mid<user_buses..., default_buses...> {
	private:
	enum class device_state {
		DEVICE_DISABLED = -1,
		DEVICE_PREINIT = 0,
		DEVICE_INITIALIZE = 1,
		DEVICE_ACTIVE = 2
	};
	public:
	explicit device_core(const char *dname, 
	    const device_core *dparent) :
	        dev_name(dname), dev_parent(dparent) {}
	~device_core() = default;
	virtual int init() const { return 0; }
	virtual int destroy() const { return 0; }
	virtual int probe() const { return 0; }
	virtual int show() const { return 0; }
	const device_core *parent() const { return dev_parent; }
	const char *name() const { return dev_name; }

	using bus_mid<user_buses..., default_buses...>::as_bus;

	device_core()= default;
	protected:

	const char * dev_name;
	const device_core * dev_parent;
	enum device_state state;
};

using default_buses = std::tuple<onewire_bus, i2c_bus, spi_bus>;
class device : public list<device>::node,
	public device_core<app::buses, default_buses> {
public:
	device(const char *name, const device *parent) :
	    device_core<app::buses, default_buses>(name, parent) { attach(); }
	device()= delete;
	~device();
	static const device *find_device(const char *name);

private:
	// Add this device into the tree
	void attach(void);
};

extern const device *root_bus;


class char_device : public device {
public:
	using device::device;
	virtual int cdev_read(char *, int) const = 0;
	virtual int cdev_write(const char *, int) const = 0;
};

class block_device : public device {
public:
	using device::device;
	virtual int get_sector(off_t, char *, int) const = 0;
	virtual int put_sector(off_t, const char *, int) const = 0;
	virtual size_t get_sector_size(void) const = 0;
	virtual size_t get_block_size(void) const = 0;
	virtual void sync(void) const = 0;
	virtual off_t get_sector_count(void) const = 0;
};

}

#define DEVICE_TYPE(type, real, name, ...) \
	type &name() {	\
		static char b[sizeof(real)] __aligned(sizeof(uintptr_t));	\
		static bool ready; \
		if (!ready) { \
			new(b) real __VA_ARGS__; \
			ready = true; \
		} \
		return *reinterpret_cast<type *>(b); \
	} \
	inline auto init_##name = []{\
		(void)name();\
		return []{};\
	}(); struct hack

#define DEVICE(type, name, ...) \
	DEVICE_TYPE(type, type, name, __VA_ARGS__)

#define	DEVICE_DECLARE(type, name) \
	type &name();

#endif // SYS_DEVICE_H
