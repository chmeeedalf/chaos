#ifndef INCLUDE_SYS_KERNEL_H
#define INCLUDE_SYS_KERNEL_H

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

#ifndef __section
#define __section(x)	__attribute__((section(x)))
#endif

#endif
