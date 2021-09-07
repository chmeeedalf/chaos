#ifndef INCLUDE_SYS_KERNEL_H
#define INCLUDE_SYS_KERNEL_H

#include <sys/cdefs.h>
#include <stdint.h>

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#define KERN_ROUND(x,y) (((x)+((y)-1))/(y))
#define MHZ (1000 * 1000)

#ifdef __cplusplus
extern "C" {
#endif
void udelay(uint32_t usec);
void set_freq(uint32_t freq);
uint32_t get_freq(void);
#ifdef __cplusplus
}
#endif

#ifndef linker_set
#define	linker_set(name, type)	\
	type __set_##name##_start __section("." name ".A");	\
	type __set_##name##_end __section("." name ".z");		\
	struct hack

#define	linker_set_elm(lname, type, name) \
	type name __section("." lname "._." #name)
#endif

#endif
