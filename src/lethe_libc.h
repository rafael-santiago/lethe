/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef LETHE_LETHE_LIBC_H
#define LETHE_LETHE_LIBC_H 1

#include <stdlib.h>
#include <sys/stat.h>

extern int (*lethe_memcmp)(const void *, const void *, size_t);

extern void *(*lethe_memcpy)(void *, const void *, size_t);

extern void *(*lethe_memset)(void *, int, size_t);

extern int (*lethe_stat)(const char *, struct stat *);

int lethe_set_memcmp(int (*usr_memcmp)(const void *, const void *, size_t));

int lethe_set_memcpy(void *(*usr_memcpy)(void *, const void *, size_t));

int lethe_set_memset(void *(*usr_memset)(void *, int, size_t));

int lethe_set_stat(int (*usr_stat)(const char *, struct stat *));

#endif
