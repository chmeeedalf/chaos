# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/machine/arm

SRCS+=	access.c \
	memchr-stub.c \
	memcpy-stub.c \
	strcpy.c \
	strlen.c

	#	aeabi_double.c \
