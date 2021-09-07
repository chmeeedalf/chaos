# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/xdr

SRCS+=	dummy.c \
	xdr.c \
	xdr_array.c \
	xdr_float.c \
	xdr_mem.c \
	xdr_private.c \
	xdr_rec.c \
	xdr_reference.c \
	xdr_sizeof.c \
	xdr_stdio.c
