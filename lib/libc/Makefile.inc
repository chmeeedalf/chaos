CFLAGS+= -DNO_EXEC
CFLAGS+= -DREENTRANT_SYSCALLS_PROVIDED
CFLAGS+= -DINTERNAL_NEWLIB
WITHOUT_SSP=
MACHINE_CPUARCH=arm
MACHINE_ARCH=arm

.include "argz/Makefile.inc"
.include "ctype/Makefile.inc"
.include "reent/Makefile.inc"
.include "errno/Makefile.inc"
#.include "iconv/Makefile.inc"
.include "locale/Makefile.inc"
.include "machine/Makefile.inc"
.include "misc/Makefile.inc"
#.include "posix/Makefile.inc"
.include "signal/Makefile.inc"
.include "search/Makefile.inc"
.include "stdio/Makefile.inc"
#.include "stdio64/Makefile.inc"
.include "stdlib/Makefile.inc"
.include "string/Makefile.inc"
.include "sys/Makefile.inc"
.include "syscalls/Makefile.inc"
.include "time/Makefile.inc"
#.include "unix/Makefile.inc"
#.include "xdr/Makefile.inc"
