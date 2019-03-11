#ifndef _HUDTDS_H_
#define _HUDTDS_H_

#define WIDTH 800
#define HEIGHT 480
#define STRIDE WIDTH * 4

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define _POSIX_C_SOURCE 200809L

#define _TENTH_SEC 1000 * 1000 * 100
#define _TEN_SEC 1000 * 1000 * 1000 * 10


extern char *strdup (__const char *__s);

typedef unsigned int uint;

#include <stdlib.h>
#include <stdio.h>

static inline char *expand_dirname(const char *d, const char *f)
{
    size_t l = snprintf(NULL, 0, "%s/%s", d, f) + 1;
    char *filename = malloc(l + 1);
    if (filename) {
        snprintf(filename, l, "%s/%s", d, f);
    }
    return filename;
}


#endif // _HUDTDS_H_
