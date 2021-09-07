# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/syscalls

SRCS+=	sysclose.c \
	sysexecve.c \
	sysfcntl.c \
	sysfork.c \
	sysfstat.c \
	sysgetpid.c \
	sysgettod.c \
	sysisatty.c \
	syskill.c \
	syslink.c \
	syslseek.c \
	sysopen.c \
	sysread.c \
	syssbrk.c \
	sysstat.c \
	systimes.c \
	sysunlink.c \
	syswait.c \
	syswrite.c
