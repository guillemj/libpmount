#if defined(__linux__)
# include "linux.c"
#elif defined(__FreeBSD_kernel__) || defined(__FreeBSD__)
# include "kfreebsd.c"
#else
# error libpmount not yet ported to this system/kernel, patches welcome
#endif
