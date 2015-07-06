# chaos
Embedded RTOS, written in C++

ChaOS is an embedded Real-Time OS, written in C++, taking advantage of templates to improve correctness.

One of the primary goals of this project is to provide some levels of guarantees, a safety belt, so to speak, that certain capabilities will be correct, if the parameters are correct.  For example, it attempts to guarantee, though type safety, that devices are only attached to specific busses.

Currently it requires an external newlib, in the same parent directory, with a basic BSD Makefile.inc, specifying parameters (CFLAGS) to build the project against.
