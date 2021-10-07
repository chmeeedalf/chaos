# vim:syntax=make:

.PATH:	${.CURDIR}/bsp

SRCS+=	pinout.c \
	platform.cpp \

CFLAGS+= -I${.CURDIR}/bsp

#CFLAGS+= -I${.CURDIR}/3rdparty/lwIP/src/include -I${.CURDIR}/bsp
#SRCS+= tivaif.c
