# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/stdio64

SRCS+=	dummy.c \
	fdopen64.c \
	fgetpos64.c \
	fopen64.c \
	freopen64.c \
	fseeko64.c \
	fsetpos64.c \
	ftello64.c \
	stdio64.c \
	tmpfile64.c
