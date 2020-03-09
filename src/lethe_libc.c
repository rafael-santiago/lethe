/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <lethe_libc.h>
#include <string.h>

int (*lethe_memcmp)(const void *, const void *, size_t) = memcmp;

void *(*lethe_memcpy)(void *, const void *, size_t) = memcpy;

void *(*lethe_memset)(void *, int, size_t) = memset;

int (*lethe_stat)(const char *, struct stat *) = stat;

int lethe_set_memcmp(int (*usr_memcmp)(const void *, const void *, size_t)) {
    int has_error = 1;
    if (usr_memcmp != NULL) {
        lethe_memcmp = usr_memcmp;
        has_error = 0;
    }
    return has_error;
}

int lethe_set_memcpy(void *(*usr_memcpy)(void *, const void *, size_t)) {
    int has_error = 1;
    if (usr_memcpy != NULL) {
        lethe_memcpy = usr_memcpy;
        has_error = 0;
    }
    return has_error;
}

int lethe_set_memset(void *(*usr_memset)(void *, int, size_t)) {
    int has_error = 1;
    if (usr_memset != NULL) {
        lethe_memset = usr_memset;
        has_error = 0;
    }
    return has_error;
}

int lethe_set_stat(int (*usr_stat)(const char *, struct stat *)) {
    int has_error = 1;

    if (usr_stat != NULL) {
        lethe_stat = usr_stat;
        has_error = 0;
    }

    return has_error;
}
