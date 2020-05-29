#ifndef PLATFORM_H 
#define PLATFORM_H

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#endif