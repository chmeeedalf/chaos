# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/misc

SRCS+=	__dprintf.c \
	ffs.c \
	fini.c \
	init.c \
	unctrl.c
