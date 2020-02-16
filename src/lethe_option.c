/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <lethe_option.h>
#include <string.h>
#include <stdio.h>

static int g_lethe_option_argc = 0;

static char **g_lethe_option_argv = NULL;

void lethe_option_set_argc_argv(int argc, char **argv) {
    g_lethe_option_argc = argc;
    g_lethe_option_argv = argv;
}

char *lethe_get_option(const char *option, char *default_value) {
    char *value = default_value;
    char temp[1024];
    char **argv, **argv_end;

    if (g_lethe_option_argc == 0 || g_lethe_option_argv == NULL) {
        goto lethe_get_option_epilogue;
    }

    snprintf(temp, sizeof(temp) - 1, "--%s=", option);

    argv = &g_lethe_option_argv[0];
    argv_end = argv + g_lethe_option_argc;

    while (argv != argv_end) {
        if (strstr(*argv, temp) == *argv) {
            value = *argv + strlen(temp);
            goto lethe_get_option_epilogue;
        }
        argv++;
    }

lethe_get_option_epilogue:

    return value;
}

int lethe_get_bool_option(const char *option, const int default_value) {
    int value = default_value;
    char temp[1024];
    char **argv, **argv_end;

    if (g_lethe_option_argc == 0 || g_lethe_option_argv == NULL) {
        goto lethe_get_bool_option_epilogue;
    }

    snprintf(temp, sizeof(temp) - 1, "--%s", option);

    argv = &g_lethe_option_argv[0];
    argv_end = argv + g_lethe_option_argc;

    while (argv != argv_end) {
        if (strcmp(*argv, temp) == 0) {
            value = 1;
            goto lethe_get_bool_option_epilogue;
        }
        argv++;
    }

lethe_get_bool_option_epilogue:

    return value;

}
