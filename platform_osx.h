#ifdef platform_h
#error "platform implementation already defined"
#else

#include <pthread.h>
#include <mach/mach_time.h>

mach_timebase_info_data_t timebase;
double conversion_factor;

typedef struct {
  pthread_t tid;
  int exit;
} plat_thread_t;

typedef void*(*plat_threadfunc_t)(void*);
#define PLAT_THREAD_FUNC(name,arg) void* name(void* arg)

#define PLATFORM_EXTRA_CR '\r'

#endif
