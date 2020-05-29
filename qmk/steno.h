#ifndef _STENO_H_
#define _STENO_H_

#include "quantum.h"
#include <stdint.h>
#include "stroke.h"

#ifdef STENO_DEBUG
#define steno_debug(format, ...) xprintf(format, ##__VA_ARGS__)
#else
#define steno_debug(...)
#endif

#endif
