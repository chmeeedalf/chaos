# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/unix

SRCS+=	basename.c \
	dirname.c \
	getcwd.c \
	getlogin.c \
	getpass.c \
	getpwent.c \
	getut.c \
	pread.c \
	pwrite.c \
	sigset.c \
	ttyname.c \
	ttyname_r.c
