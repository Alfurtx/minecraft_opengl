#ifndef UTILS_H_
#define UTILS_H_

#include "FastNoiseLite.h"
#include "direction.h"
#include "stb_ds.h"
#include "stb_image.h"
#include "noise1234.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cglm/call.h>
#include <cglm/cglm.h>
#include <pthread.h>
#include <time.h>

#define max(a, b)                       \
        ({                              \
                __typeof__(a) _a = (a); \
                __typeof__(b) _b = (b); \
                _a > _b ? _a : _b;      \
        })

#define min(a, b)                       \
        ({                              \
                __typeof__(a) _a = (a); \
                __typeof__(b) _b = (b); \
                _a < _b ? _a : _b;      \
        })

#define clamp(x, mn, mx)                   \
        ({                                 \
                __typeof__(x)  _x  = (x);  \
                __typeof__(mn) _mn = (mn); \
                __typeof__(mx) _mx = (mx); \
                max(_mn, min(_mx, _x));    \
        })

#define NS_PER_SECOND (1000000000)
#define NS_PER_MS (1000000)
#define NOW()                                               \
        ({                                                  \
                struct timespec ts;                         \
                timespec_get(&ts, TIME_UTC);                \
                ((ts.tv_sec * NS_PER_SECOND) + ts.tv_nsec); \
        })

#endif // UTILS_H_
