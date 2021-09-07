# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/posix

SRCS+=	_isatty.c \
	closedir.c \
	collate.c \
	collcmp.c \
	creat.c \
	engine.c \
	execl.c \
	execle.c \
	execlp.c \
	execv.c \
	execve.c \
	execvp.c \
	fnmatch.c \
	glob.c \
	isatty.c \
	opendir.c \
	popen.c \
	posix_spawn.c \
	readdir.c \
	readdir_r.c \
	regcomp.c \
	regerror.c \
	regexec.c \
	regfree.c \
	rewinddir.c \
	scandir.c \
	seekdir.c \
	sleep.c \
	telldir.c \
	usleep.c \
	wordexp.c \
	wordfree.c
