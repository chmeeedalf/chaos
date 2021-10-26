# vim:syntax=make:

.PATH: ${.CURDIR}/driverlib

SRCS+=	gpio.c \
	interrupt.c \
	i2c.c \
	sysctl.c
