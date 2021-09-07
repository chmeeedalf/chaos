# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/signal

SRCS+=	psignal.c \
	raise.c \
	signal.c
