# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/time

SRCS+=	asctime.c \
	asctime_r.c \
	clock.c \
	ctime.c \
	ctime_r.c \
	difftime.c \
	gettzinfo.c \
	gmtime.c \
	gmtime_r.c \
	lcltime.c \
	lcltime_r.c \
	mktime.c \
	mktm_r.c \
	strftime.c \
	strptime.c \
	time.c \
	tzlock.c \
	tzset.c \
	tzset_r.c \
	tzvars.c \
	wcsftime.c
