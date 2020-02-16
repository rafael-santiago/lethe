/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#ifndef LETHE_LETHE_OPTION_H
#define LETHE_LETHE_OPTION_H 1

void lethe_option_set_argc_argv(int argc, char **argv);

char *lethe_get_option(const char *option, char *default_value);

int lethe_get_bool_option(const char *option, const int default_value);

#endif
