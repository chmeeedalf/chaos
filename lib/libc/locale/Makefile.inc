# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/locale

SRCS+=	fix_grouping.c \
	lctype.c \
	ldpart.c \
	lmessages.c \
	lmonetary.c \
	lnumeric.c \
	locale.c \
	nl_langinfo.c \
	timelocal.c
