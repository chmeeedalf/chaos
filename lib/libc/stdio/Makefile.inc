# vim:syntax=make:

.PATH: ${LIBC_SRCTOP}/stdio

SRCS+=	asiprintf.c \
	asniprintf.c \
	asnprintf.c \
	asprintf.c \
	clearerr.c \
	diprintf.c \
	dprintf.c \
	fclose.c \
	fcloseall.c \
	fdopen.c \
	feof.c \
	ferror.c \
	fflush.c \
	fgetc.c \
	fgetpos.c \
	fgets.c \
	fgetwc.c \
	fgetws.c \
	fileno.c \
	findfp.c \
	fiprintf.c \
	fiscanf.c \
	flags.c \
	fmemopen.c \
	fopen.c \
	fopencookie.c \
	fprintf.c \
	fpurge.c \
	fputc.c \
	fputs.c \
	fputwc.c \
	fputws.c \
	fread.c \
	freopen.c \
	fscanf.c \
	fseek.c \
	fseeko.c \
	fsetpos.c \
	ftell.c \
	ftello.c \
	funopen.c \
	fvwrite.c \
	fwalk.c \
	fwide.c \
	fwprintf.c \
	fwrite.c \
	fwscanf.c \
	getc.c \
	getc_u.c \
	getchar.c \
	getchar_u.c \
	getdelim.c \
	getline.c \
	gets.c \
	getw.c \
	getwc.c \
	getwchar.c \
	iprintf.c \
	iscanf.c \
	makebuf.c \
	mktemp.c \
	open_memstream.c \
	perror.c \
	printf.c \
	putc.c \
	putc_u.c \
	putchar.c \
	putchar_u.c \
	puts.c \
	putw.c \
	putwc.c \
	putwchar.c \
	refill.c \
	remove.c \
	rename.c \
	rewind.c \
	rget.c \
	scanf.c \
	sccl.c \
	setbuf.c \
	setbuffer.c \
	setlinebuf.c \
	setvbuf.c \
	siprintf.c \
	siscanf.c \
	sniprintf.c \
	snprintf.c \
	sprintf.c \
	sscanf.c \
	stdio.c \
	swprintf.c \
	swscanf.c \
	tmpfile.c \
	tmpnam.c \
	ungetc.c \
	ungetwc.c \
	vasiprintf.c \
	vasniprintf.c \
	vasnprintf.c \
	vasprintf.c \
	vdiprintf.c \
	vdprintf.c \
	vfscanf.c \
	vfwprintf.c \
	vfwscanf.c \
	viprintf.c \
	viscanf.c \
	vprintf.c \
	vscanf.c \
	vsiprintf.c \
	vsiscanf.c \
	vsniprintf.c \
	vsnprintf.c \
	vsprintf.c \
	vsscanf.c \
	vswprintf.c \
	vswscanf.c \
	vwprintf.c \
	vwscanf.c \
	wbuf.c \
	wprintf.c \
	wscanf.c \
	wsetup.c

GENSRCS= svfprintf.c vfiprintf.c svfiprintf.c
CFLAGS+=-I${LIBC_SRCTOP}/stdio

.if "${INTEGER_ONLY}" == "yes"
SRCS+= vfiprintf.c \
	svfiprintf.c
.else
SRCS+= svfprintf.c \
	vfprintf.c
.endif

svfprintf.c: vfprintf.c
	echo "#define STRING_ONLY" > ${.TARGET}
	echo '#include "vfprintf.c"' >> ${.TARGET}

vfiprintf.c: vfprintf.c
	echo "#define INTEGER_ONLY" > ${.TARGET}
	echo '#include "vfprintf.c"' >> ${.TARGET}

svfiprintf.c: vfprintf.c
	echo "#define STRING_ONLY" > ${.TARGET}
	echo "#define INTEGER_ONLY" >> ${.TARGET}
	echo '#include "vfprintf.c"' >> ${.TARGET}
