#ifndef _HUDTDS_H_
#define _HUDTDS_H_

#define WIDTH 800
#define HEIGHT 480
#define STRIDE WIDTH * 4

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define _POSIX_C_SOURCE 200809L

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
