# vim:syntax=make:

.PATH: ${.CURDIR}/drivers/1w

SRCS+=	1w.cpp \
	ds18b20_1w.cpp \
	ds2482_1w.cpp
#bitbang_1w.cpp \
