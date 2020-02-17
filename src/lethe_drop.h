/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#ifndef LETHE_LETHE_DROP_H
#define LETHE_LETHE_DROP_H 1

#include <lethe_types.h>

#define lethe_drop(pattern, dtype...) lethe_drop_pattern(pattern, dtype)

int lethe_drop_pattern(const char *filepath, const lethe_drop_type dtype, ...);

int lethe_set_rename_nr(const int value);

int lethe_set_stat(lethe_stat func_addr);

#endif
