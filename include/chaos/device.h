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

/* C Preprocessor abuse */
#include <chaos/cpp.h>

#ifndef SYS_DEVICE_H
#define SYS_DEVICE_H

namespace chaos {
enum class device_state {
	DEVICE_DISABLED = -1,
	DEVICE_PREINIT = 0,
	DEVICE_INITIALIZE = 1,
	DEVICE_ACTIVE = 2
};

struct device_run {
	enum device_state state;
};

struct no_softc_t{};
struct root_device{};

static const root_device *root_bus = nullptr;

template <typename T, typename P> class device;
int initialize_device(const chaos::device<void,const chaos::device<void,void>> *dev);
int initialize_devices();

/*
 * All the virtual methods are const qualified, because they act on the mutable
 * data in the softc.
 */
template<typename T, typename P>
class device {
	public:
	constexpr device(int did, const char *dname, const char *ddescr,
	    T *dsoftc, const P *dparent, device_run *drun) :
	        dev_id(did), dev_name(dname), dev_descr(ddescr),
	        dev_parent(dparent), dev_softc(dsoftc), dev_run(drun) {}
	virtual int init() const { return 0; }
	virtual int destroy() const { return 0; }
	virtual int probe() const { return 0; }
	virtual int show() const { return 0; }
	const P *parent() const { return dev_parent; }
	const char *name() const { return dev_name; }
	const char *descr() const { return dev_descr; }

	typedef T softc_type;
	protected:
	friend int initialize_device(const device<void,const device<void,void>> *dev);

	struct softc_base { };
	int dev_id;
	const char *dev_name;
	const char *dev_descr;
	const P *dev_parent;
	softc_type *dev_softc;
	device_run *dev_run;
};

template<typename T, typename P>
class char_device : public device<T, P> {
public:
	using device<T, P>::device;
	virtual int cdev_read(char *, int) const;
	virtual int cdev_write(const char *, int) const;
};

/* Convention is:
 * class
 *  - class_softc
 *  - class_methods
 */

#define ___DEVICE(cls, name, sc, descr, parent, ...) \
	static const int __CONCAT(name,_id) __section(".devid") = (int)&__CONCAT(name,_id); \
	static sc __CONCAT(name,_softc){__VA_ARGS__}; \
	static chaos::device_run __CONCAT(name,_devrun) {chaos::device_state::DEVICE_PREINIT}; \
	static const cls __CONCAT(name,_dev) __attribute__((used)){ \
		(int)&__CONCAT(name,_id),\
		#name, \
		descr, \
		&__CONCAT(name,_softc), \
		parent, \
		&__CONCAT(name,_devrun), \
	}; \
	static const cls *name __attribute__((used)) __section(".device") = &__CONCAT(name, _dev); \
	struct __hack
#define __DEVICE(cls, name, sc, descr, parent, ...) \
		___DEVICE(cls, name, sc, descr, parent, __VA_ARGS__)
#define _DEVICE(cls, name, sc, descr, parent, ...) \
		__DEVICE(cls, name, sc, descr, parent, __VA_ARGS__)
#define DEVICE(cls, name, descr, parent, ...) \
		_DEVICE(cls, name, cls::softc_type, descr, parent, __VA_ARGS__)

#define DRIVER_PARENT_GEN(p)	const p *__CONCAT(parent_,__COUNTER__);
#define DRIVER(cls, ...) \
	_Static_assert(sizeof(cls) == sizeof(device), #cls " doesn't match size of device"); \
	struct __hack

}

#endif // SYS_DEVICE_H
