/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <lethe_mkpath.h>
#include <lethe_libc.h>

char *lethe_mkpath(char *buf, const size_t buf_size, const char *a, const size_t a_size, const char *b, const size_t b_size) {
    const char *ap, *ap_end, *bp, *bp_end;
    size_t m;

    if (buf == NULL || a == NULL || b == NULL || buf_size == 0 || (a_size + b_size + 2) > buf_size - 1) {
        return NULL;
    }

    lethe_memset(buf, 0, buf_size);

#if defined(__unix__)
    ap = a;
    ap_end = a + a_size - (a_size > 0);

    while (ap_end != ap && *ap_end == '/') {
        ap_end--;
    }

    m = ap_end - a + (a_size > 0);

    if (m > 0) {
        lethe_memcpy(buf, a, m);
    }

    bp = b;
    bp_end = b + b_size;

    while (bp != bp_end && *bp == '/') {
        bp++;
    }

    *(buf + m) = (m > 0 && bp != bp_end) ? '/' : 0;

    if (bp != bp_end) {
        lethe_memcpy(buf + m + (m > 0), bp, bp_end - bp);
    }
#elif defined(_WIN32)
    ap = a;
    ap_end = a + a_size - (a_size > 0);

    while (ap_end != ap && (*ap_end == '\\' ||
                            *ap_end == '/')) {
        ap_end--;
    }

    m = ap_end - a + (a_size > 0);

    if (m > 0) {
        lethe_memcpy(buf, a, m);
    }

    bp = b;
    bp_end = b + b_size;

    while (bp != bp_end && (*bp == '\\' ||
                            *bp == '/')) {
        bp++;
    }

    *(buf + m) = (m > 0 && bp != bp_end) ? '\\' : 0;

    if (bp != bp_end) {
        lethe_memcpy(buf + m + (m > 0), bp, bp_end - bp);
    }
#else
# error Some code wanted.
#endif

    return buf;
}
