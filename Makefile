LIBC_SRCTOP=${.CURDIR}/../newlib/libc

.include "../newlib/Makefile.inc"

CXX=clang
CFLAGS+=-I${LIBC_SRCTOP}/include
CFLAGS+=-DTARGET_IS_TM4C129_RA1
CFLAGS+=-DPART_TM4C1294NCPDT
CFLAGS+=-I${.CURDIR}
CFLAGS+=-I${.CURDIR}/include
CFLAGS+=-Dgcc -std=gnu11
# Only while using FreeRTOS
CFLAGS+=-I${.CURDIR}/3rdparty/FreeRTOS/Source/include -DRTOS_FREERTOS
CFLAGS+=-I${.CURDIR}/3rdparty/FreeRTOS/Source/portable/GCC/ARM_CM4F

CXXFLAGS=${CFLAGS} -std=gnu++11 -fno-exceptions -fno-rtti
LDFLAGS+=-L${LIBC_SRCTOP} --gc-sections

PROG=hello.axf
SRCS=hello.cpp
NO_MAN=

LIBC=${LIBC_SRCTOP}/libc.a
LIBM=
LIBGCC=${.CURDIR}/../libcompiler_rt/libcompiler_rt.a
#EXTRA_LIBS=libdriver.a

.include "bsp/Makefile.inc"
.include "driverlib/Makefile.inc"
#.include "3rdparty/onewire/Makefile.inc"
.include "drivers/Makefile.inc"
.include "kernel/Makefile.inc"
.include "util/Makefile.inc"
#.include "net/Makefile.inc"

ENTRY_hello=ResetISR

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
	          ${LDFLAGS} -o ${@} $(filter %.o %.a, ${^})                  \
	          '${LIBM}' '${LIBC}' '${LIBGCC}';                            \
	 fi;                                                                  \
	${LD} -T $${ldname}                                                   \
	      --entry ${ENTRY_${.TARGET:.axf=}}                               \
	      ${LDFLAGSgcc_${.TARGET:.axf=}}                                  \
	      ${LDFLAGS} -o ${.TARGET} ${OBJS}                                \
	      ${EXTRA_LIBS} ${LIBM} ${LIBC} ${LIBGCC} ${LIBC}
	@${OBJCOPY} -O binary ${.TARGET} ${.TARGET:.axf=.bin}

.include <bsd.prog.mk>
