# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/search

SRCS+=	bsearch.c \
	hash.c \
	hash_bigkey.c \
	hash_buf.c \
	hash_func.c \
	hash_log2.c \
	hash_page.c \
	hcreate.c \
	hcreate_r.c \
	qsort.c \
	tdelete.c \
	tdestroy.c \
	tfind.c \
	tsearch.c \
	twalk.c
