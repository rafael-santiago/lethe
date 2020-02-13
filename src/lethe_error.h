/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#ifndef LETHE_LETHE_ERROR_H
#define LETHE_LETHE_ERROR_H 1

#include <lethe_types.h>
#include <stdlib.h>

void lethe_set_error_code(const lethe_error_code code);

void lethe_set_error_last_filepath(const char *filepath);

char *lethe_get_last_error(char *buf, const size_t buf_size);

#endif
