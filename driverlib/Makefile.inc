# vim:syntax=make:

.PATH: ${.CURDIR}/driverlib

SRCS+=	gpio.c \
	interrupt.c \
	cpu.c \
	i2c.c \
	sysctl.c
