/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <lethe_mkpath.h>
#include <string.h>
#include <stdio.h>

char *lethe_mkpath(char *buf, const size_t buf_size, char *a, const size_t a_size, char *b, const size_t b_size) {
    char *sep;

    if (a == NULL || b == NULL || (a_size + b_size + 2) > buf_size - 1) {
        return NULL;
    }

#if defined(__unix__)
    sep = (a[a_size - (a_size > 0)] != '/' && b[0] != '/') ? "/" : "";
    memset(buf, 0, buf_size);
    snprintf(buf, buf_size - 1, "%s%s%s", a, sep, b);
#else
# error Some code wanted.
#endif

    return buf;
}
