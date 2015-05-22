#include <config.h>

#include <reent.h>
#include <errno.h>
#include <sys/lock.h>
#include <sys/types.h>
#include <sys/thread.h>

long __stack_chk_guard[8] = {0};

long reqdiff;
long heap_top;
long hsize;

void
__stack_chk_fail(void)
{
	*(volatile uint32_t *)0 = 0; /* Force a hard fault */
}

void
*_sbrk_r(struct _reent *reent, ptrdiff_t diff)
{
	void *ret;

	diff = ((diff + sizeof(uint32_t) - 1) / sizeof(uint32_t));

	reqdiff = diff;
	heap_top = curthread->thr_run->thr_heap_top;
	hsize = curthread->thr_hsize;
	if (curthread->thr_run->thr_heap_top + diff < 0) {
		reent->_errno = EINVAL;
		ret = (void *)-1;
	}
	else if (curthread->thr_run->thr_heap_top + diff > (curthread->thr_hsize / sizeof(uintptr_t))) {
		reent->_errno = ENOMEM;
		ret = (void *)-1;
	}
	else {
		ret = &curthread->thr_heap[curthread->thr_run->thr_heap_top];
		curthread->thr_run->thr_heap_top += diff;
	}

	return ret;
}

int
_fstat_r(struct _reent *reent, int fd, struct stat *sb)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_close_r(struct _reent *reent, int fd)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_isatty_r(struct _reent *reent, int fd)
{
	reent->_errno = ENOSYS;
	return -1;
}

off_t
_lseek_r(struct _reent *reent, int fd, off_t offset, int whence)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_read_r(struct _reent *reent, int fd, void *buf, size_t nb)
{
	reent->_errno = ENOSYS;
	return -1;
}

ssize_t
_write_r(struct _reent *reent, int fd, const void *buf, size_t nb)
{
	reent->_errno = ENOSYS;
	return -1;
}

struct _reent *
_getreent(void)
{
	return &curthread->thr_run->thr_reent;
}
