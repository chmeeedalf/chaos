# vim:syntax=make:

.PATH:	${.CURDIR}/bsp

SRCS+=	libc_support.c \
	pinout.c \
	platform.cpp \
	startup_gcc.c

#CFLAGS+= -I${.CURDIR}/3rdparty/lwIP/src/include -I${.CURDIR}/bsp
#SRCS+= tivaif.c
