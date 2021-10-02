
#ifndef STDDEF_H
#define STDDEF_H
#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long;
#endif
typedef __SIZE_TYPE__ size_t;
#ifndef __cplusplus
typedef unsigned int wchar_t;
#endif
typedef int wint_t;
typedef long ptrdiff_t;

typedef long long __attribute__((aligned(_Alignof(long long)))) max_align_t;

#define NULL ((void *)0)
#define	offsetof(type, member)	__offsetof(type, member)
#define __offsetof(type, member) __builtin_offsetof(type, member)
#endif
