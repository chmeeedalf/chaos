#include <sys/cdefs.h>

#ifndef SYS_CPP_H
#define SYS_CPP_H

#define CPP_MAX 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
#define CPP_COUNT_2(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define CPP_COUNT_1(...) CPP_COUNT_2(__VA_ARGS__)
#define CPP_COUNT(...) CPP_COUNT_1(__VA_ARGS__, CPP_MAX)

#define CPP_0(_1, ...) _1
#define CPP_X(_1, ...) (__VA_ARGS__)

#define FOR_EACH_1(mod, a) mod(CPP_0 a)
#define FOR_EACH_2(mod, a) FOR_EACH_1(mod, CPP_X a) mod(CPP_0 a)
#define FOR_EACH_3(mod, a) FOR_EACH_2(mod, CPP_X a) mod(CPP_0 a)
#define FOR_EACH_4(mod, a) FOR_EACH_3(mod, CPP_X a) mod(CPP_0 a)
#define FOR_EACH_5(mod, a) FOR_EACH_4(mod, CPP_X a) mod(CPP_0 a)
#define FOR_EACH_6(mod, a) FOR_EACH_5(mod, CPP_X a) mod(CPP_0 a)
#define FOR_EACH_7(mod, a) FOR_EACH_6(mod, CPP_X a) mod(CPP_0 a)
#define FOR_EACH_8(mod, a) FOR_EACH_7(mod, CPP_X a) mod(CPP_0 a)
#define FOR_EACH_9(mod, a) FOR_EACH_8(mod, CPP_X a) mod(CPP_0 a)
#define FOR_EACH_10(mod, a) FOR_EACH_9(mod, CPP_X a) mod(CPP_0 a)

#define FOR_EACH(mod, ...)  \
	__CONCAT(FOR_EACH_, CPP_COUNT(__VA_ARGS__))(mod, (__VA_ARGS__))

#define CPP_CONCAT_EACH_1(mod, a) mod(CPP_0 a)
#define CPP_CONCAT_EACH_2(mod, a) CPP_CONCAT_EACH_1(mod, CPP_X a), mod(CPP_0 a)
#define CPP_CONCAT_EACH_3(mod, a) CPP_CONCAT_EACH_2(mod, CPP_X a), mod(CPP_0 a)
#define CPP_CONCAT_EACH_4(mod, a) CPP_CONCAT_EACH_3(mod, CPP_X a), mod(CPP_0 a)
#define CPP_CONCAT_EACH_5(mod, a) CPP_CONCAT_EACH_4(mod, CPP_X a), mod(CPP_0 a)
#define CPP_CONCAT_EACH_6(mod, a) CPP_CONCAT_EACH_5(mod, CPP_X a), mod(CPP_0 a)
#define CPP_CONCAT_EACH_7(mod, a) CPP_CONCAT_EACH_6(mod, CPP_X a), mod(CPP_0 a)
#define CPP_CONCAT_EACH_8(mod, a) CPP_CONCAT_EACH_7(mod, CPP_X a), mod(CPP_0 a)
#define CPP_CONCAT_EACH_9(mod, a) CPP_CONCAT_EACH_8(mod, CPP_X a), mod(CPP_0 a)
#define CPP_CONCAT_EACH_10(mod, a) CPP_CONCAT_EACH_9(mod, CPP_X a), mod(CPP_0 a)

#define CPP_CONCAT_EACH(mod, ...)  \
	__CONCAT(CPP_CONCAT_EACH_, CPP_COUNT(__VA_ARGS__))(mod, (__VA_ARGS__))

#define CPP_CONCAT_EACH_INDEX_1(mod, a) mod(CPP_0 a, 10-CPP_COUNT(a))
#define CPP_CONCAT_EACH_INDEX_2(mod, a) CPP_CONCAT_EACH_INDEX_1(mod, CPP_X a), mod(CPP_0 a, 10-CPP_COUNT(a))
#define CPP_CONCAT_EACH_INDEX_3(mod, a) CPP_CONCAT_EACH_INDEX_2(mod, CPP_X a), mod(CPP_0 a, 10-CPP_COUNT(a))
#define CPP_CONCAT_EACH_INDEX_4(mod, a) CPP_CONCAT_EACH_INDEX_3(mod, CPP_X a), mod(CPP_0 a, 10-CPP_COUNT(a))
#define CPP_CONCAT_EACH_INDEX_5(mod, a) CPP_CONCAT_EACH_INDEX_4(mod, CPP_X a), mod(CPP_0 a, 10-CPP_COUNT(a))
#define CPP_CONCAT_EACH_INDEX_6(mod, a) CPP_CONCAT_EACH_INDEX_5(mod, CPP_X a), mod(CPP_0 a, 10-CPP_COUNT(a))
#define CPP_CONCAT_EACH_INDEX_7(mod, a) CPP_CONCAT_EACH_INDEX_6(mod, CPP_X a), mod(CPP_0 a, 10-CPP_COUNT(a))
#define CPP_CONCAT_EACH_INDEX_8(mod, a) CPP_CONCAT_EACH_INDEX_7(mod, CPP_X a), mod(CPP_0 a, 10-CPP_COUNT(a))
#define CPP_CONCAT_EACH_INDEX_9(mod, a) CPP_CONCAT_EACH_INDEX_8(mod, CPP_X a), mod(CPP_0 a, 10-CPP_COUNT(a))
#define CPP_CONCAT_EACH_INDEX_10(mod, a) CPP_CONCAT_EACH_INDEX_9(mod, CPP_X a), mod(CPP_0 a, 10-CPP_COUNT(a))

#define CPP_CONCAT_EACH_INDEX(mod, ...)  \
	__CONCAT(CPP_CONCAT_EACH_INDEX_, CPP_COUNT(__VA_ARGS__))(mod, (__VA_ARGS__))
#endif
