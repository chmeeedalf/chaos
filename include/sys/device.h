#include <sys/cdefs.h>
#include <sys/types.h>

/* C Preprocessor abuse */
#include <sys/cpp.h>

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
	virtual int init() const;
	virtual int destroy() const;
	virtual int probe() const;
	virtual int show() const;
	const P *parent() const { return dev_parent; }
	const char *name() const { return dev_name; }

	protected:
	friend int initialize_device(const chaos::device<void,const chaos::device<void,void>> *dev);
	int dev_id;
	const char *dev_name;
	const char *dev_descr;
	const P *dev_parent;
	T *dev_softc;
	device_run *dev_run;
};

template<typename T, typename P>
class char_device {
	public:
	virtual int cdev_read(char *, int) const;
	virtual int cdev_write(char *, int) const;
};

/* Convention is:
 * class
 *  - class_softc
 *  - class_methods
 */

#define ___DEVICE(cls, name, sc, descr, parent, ...) \
	static const int __CONCAT(name,_id) __attribute__((section(".devid"))) = (int)&__CONCAT(name,_id); \
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
	static const cls *name __attribute__((used)) __attribute__((section(".device"))) = &__CONCAT(name, _dev); \
	struct __hack
#define __DEVICE(cls, name, sc, descr, parent, methods, ...) \
		___DEVICE(cls, name, sc, descr, parent, methods, __VA_ARGS__)
#define _DEVICE(cls, name, sc, descr, parent, methods, ...) \
		__DEVICE(cls, name, sc, descr, parent, methods, __VA_ARGS__)
#define DEVICE(cls, name, descr, parent, methods, ...) \
		_DEVICE(cls, name, cls##_softc, descr, parent, methods, __VA_ARGS__)

#define DRIVER_PARENT_GEN(p)	const p *__CONCAT(parent_,__COUNTER__);
#define DRIVER(cls, ...) \
	_Static_assert(sizeof(cls) == sizeof(device), #cls " doesn't match size of device"); \
	struct __hack

}

#endif // SYS_DEVICE_H
