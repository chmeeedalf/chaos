
#ifndef STDDEF_H
#define STDDEF_H
typedef unsigned long size_t;
#ifndef __cplusplus
typedef unsigned int wchar_t;
#endif
typedef int wint_t;
typedef long ptrdiff_t;

#define NULL ((void *)0)
#define	offsetof(type, member)	__offsetof(type, member)
#define __offsetof(type, member) __builtin_offsetof(type, member)
#endif
