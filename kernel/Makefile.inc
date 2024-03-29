# vim:syntax=make:
.PATH:	${.CURDIR}/kernel

SRCS+=	libc_support.cpp \
	thread.cpp	\
	bss_vars.cpp	\
	device.cpp

FREERTOS=${.CURDIR}/3rdparty/FreeRTOS
.PATH:	${FREERTOS}

FREERTOS_PORT=	GCC/ARM_CM3

CFLAGS+= -I${FREERTOS}/include
CFLAGS+= -I${FREERTOS}/portable/${FREERTOS_PORT}

SRCS+=	list.c	\
	queue.c	\
	tasks.c	\
	timers.c

.PATH:	${FREERTOS}/portable/${FREERTOS_PORT}

SRCS+=	port.c
