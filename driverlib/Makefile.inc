# vim:syntax=make:

.PATH: ${.CURDIR}/driverlib

SRCS+=	\
	flash.c \
	gpio.c \
	interrupt.c \
	i2c.c \
	sysctl.c
