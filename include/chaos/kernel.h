#ifndef INCLUDE_SYS_KERNEL_H
#define INCLUDE_SYS_KERNEL_H

#include <sys/cdefs.h>
#include <stdint.h>

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#define MHZ (1000 * 1000)

__BEGIN_DECLS
void udelay(uint32_t usec);
void set_freq(uint32_t freq);
uint32_t get_freq(void);
void init_chaos(void);

__END_DECLS

#ifndef linker_set
#define	linker_set(name, type)	\
	type __CONCAT(__set_, __CONCAT(name, _start)) __section("." #name ".A") __used = {};	\
	type __CONCAT(__set_, __CONCAT(name, _end)) __section("." #name ".z") __used = {};	\
	struct hack

#define	linker_set_elm(lname, type, name) \
	type name __section("." lname "._." #name)
#endif

#define	SINGLETON(type, name, ...) \
	inline type &name() { \
		static char b[sizeof(type)] __aligned(sizeof(uintptr_t)); \
		static bool ready; \
		if (!ready) \
			new(b) type __VA_ARGS__; \
		ready = true; \
		return (*reinterpret_cast<type *>(b)); \
	} \
	inline auto init_##name = []{\
		(void)name();\
		return []{};\
	}(); struct hack

#endif
