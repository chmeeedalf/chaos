LIBC_SRCTOP=${.CURDIR}/lib/libc

CSTD=gnu17
CXXSTD=gnu++20
ABI=armelf
ABIPREFIX=arm-none-eabi
CPU=arm
CC=clang
CXX=clang++
AR=${ABIPREFIX}-ar
AS=${ABIPREFIX}-as
LD=${ABIPREFIX}-ld
RANLIB=${ABIPREFIX}-ranlib
STRINGS=${ABIPREFIX}-strings
OBJCOPY=${ABIPREFIX}-objcopy

CFLAGS+= -mthumb -mcpu=cortex-m4 -target arm-unknown-eabi -nostdinc -nostdlib
CFLAGS+= -mfloat-abi=hard
CFLAGS+= -Os -ggdb
CFLAGS+=-Wno-incompatible-library-redeclaration
CFLAGS+=-Wno-attributes
CFLAGS+= -ffunction-sections
CFLAGS+= -fno-stack-protector
CFLAGS+= -D_DYNAMIC_REENT_
CFLAGS+= -fno-use-cxa-atexit
LDFLAGS+= -m ${ABI}

CFLAGS+=-isystem ${LIBC_SRCTOP}/include
CFLAGS+=-DTARGET_IS_TM4C129_RA1 -DPART_TM4C1294NCPDT
CFLAGS+=-Wall
CFLAGS+=-I${.CURDIR}
CFLAGS+=-I${.CURDIR}/include
CFLAGS+=-Dgcc
CFLAGS+=-I${.CURDIR}/3rdparty/lwIP/src/include
CFLAGS+= -Wno-unused-function

.ifdef CSTD
CFLAGS+= -std=${CSTD}
.endif
.ifdef CXXSTD
CXXFLAGS+= -std=${CXXSTD}
.endif

CXXFLAGS+= -fno-exceptions
CXXFLAGS+= -I${.CURDIR}/include/c++/v1
CXXFLAGS+=-ffreestanding
LDFLAGS+=-L${LIBC_SRCTOP} --gc-sections

MK_MAN=no
# Works around issues with abusing bsd.prog.mk
MK_DEBUG_FILES=no

LIBM=
LIBGCC=${.CURDIR}/../libcompiler_rt/libcompiler_rt.a

.include "app/Makefile.inc"
.include "arch/Makefile.inc"
.include "bsp/Makefile.inc"
.include "cxx/Makefile.inc"
.include "driverlib/Makefile.inc"
.include "drivers/Makefile.inc"
.include "kernel/Makefile.inc"
.include "util/Makefile.inc"
.include "lib/Makefile.inc"

ENTRY_hello=_start

CLEANFILES+=${PROG:.axf=.bin}

${PROG}:
	@if [ 'x${SCATTERgcc_${notdir ${@:.axf=}}}' = x ];                    \
	 then                                                                 \
	     ldname="${.CURDIR}/standalone.ld";                               \
	 else                                                                 \
	     ldname="${SCATTERgcc_${notdir ${@:.axf=}}}";                     \
	 fi;                                                                  \
	 if [ 'x${VERBOSE}' = x ];                                            \
	 then                                                                 \
	     echo "  LD    ${@} ${LNK_SCP}";                                  \
	 else                                                                 \
	     echo ${LD} -T $${ldname}                                         \
	          --entry ${ENTRY_${.TARGET:.axf=}}                           \
	          ${LDFLAGSgcc_${notdir ${@:.axf=}}}                          \
	          ${LDFLAGS} -o ${@} $(filter %.o, ${^})                      \
	          '${LIBM}' '${LIBGCC}';                                      \
	 fi;                                                                  \
	${LD} -T $${ldname}                                                   \
	      --entry ${ENTRY_${.TARGET:.axf=}}                               \
	      ${LDFLAGSgcc_${.TARGET:.axf=}}                                  \
	      ${LDFLAGS} -o ${.TARGET} ${OBJS}                                \
	      ${EXTRA_LIBS} ${LIBM} ${LIBGCC}
	${OBJCOPY} -O binary ${.TARGET} ${.TARGET:.axf=.bin}

.include <bsd.prog.mk>
