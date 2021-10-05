#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

#include <chaos/_lock.h>
#include <stdbool.h>

typedef struct _chaos_lock _LOCK_T;
typedef struct _chaos_lock _LOCK_RECURSIVE_T;
 
#include <_ansi.h>

#define __LOCK_INIT(cls,lock) 						\
	cls struct _chaos_lock lock __section(".locks");	\
	struct hack

#define __LOCK_INIT_RECURSIVE(cls,lock)					\
	cls struct _chaos_lock lock __section(".recursive_locks");	\
	struct hack

void chaos_lock_init(_LOCK_T *lock);
void chaos_lock_init_recursive(_LOCK_RECURSIVE_T *lock);
void chaos_lock_close(_LOCK_T *lock);
void chaos_lock_close_recursive(_LOCK_RECURSIVE_T *lock);
void chaos_lock_acquire(_LOCK_T *lock);
void chaos_lock_acquire_recursive(_LOCK_RECURSIVE_T *lock);
bool chaos_lock_try_acquire(_LOCK_T *lock);
bool chaos_lock_try_acquire_recursive(_LOCK_RECURSIVE_T *lock);
void chaos_lock_release(_LOCK_T *lock);
void chaos_lock_release_recursive(_LOCK_RECURSIVE_T *lock);

#define __lock_init(lock) chaos_lock_init(&lock)
#define __lock_init_recursive(lock) chaos_lock_init_recursive(&lock)
#define __lock_close(lock) chaos_lock_close(&lock)
#define __lock_close_recursive(lock) chaos_lock_close_recursive(&lock)
#define __lock_acquire(lock) chaos_lock_acquire(&lock)
#define __lock_acquire_recursive(lock) chaos_lock_acquire_recursive(&lock)
#define __lock_try_acquire(lock) chaos_lock_try_acquire(&lock)
#define __lock_try_acquire_recursive(lock) chaos_lock_try_acquire_recursive(&lock)
#define __lock_release(lock) chaos_lock_release(&lock)
#define __lock_release_recursive(lock) chaos_lock_release_recursive(&lock)

#endif /* __SYS_LOCK_H__ */
