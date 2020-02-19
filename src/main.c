/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <lethe.h>
#include <lethe_option.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int do_drop(void);

static int do_drop_help(void);

static int do_help(void);

static int do_help_help(void); // INFO(Rafael): Children will love this function name...

static int do_man(void);

static int do_man_help(void);

static int do_version(void);

static int lethe_exec(int argc, char **argv);

static struct lethe_tool_commands_ctx {
    const char *ucmd;
    int (*task)(void);
    int (*help)(void);
} g_lethe_tool_commands[] = {
    { "drop",    do_drop,    do_drop_help    },
    { "help",    do_help,    do_help_help    },
    { "man",     do_man,     do_man_help     },
    { "version", do_version, NULL            }
};

static size_t g_lethe_tool_commands_nr = sizeof(g_lethe_tool_commands[0]) / sizeof(g_lethe_tool_commands);

const char *g_lethe_tool_version = "v1";

int main(int argc, char **argv) {
    return lethe_exec(argc, argv);
}

static int lethe_exec(int argc, char **argv) {
    char *ucmd;
    int has_error = 1;
    struct lethe_tool_commands_ctx *cmd, *cmd_end;

    lethe_option_set_argc_argv(argc, argv);

    if (lethe_get_bool_option("version", 0)) {
        // INFO(Rafael): Keeping --version people happy, in silent mode; or keeping those people in silent mode.
        has_error = do_version();
        goto lethe_exec_epilogue;
    }

    if ((ucmd = lethe_get_ucmd()) == NULL) {
        fprintf(stderr, "Well, what are intending to do? Try to call me again by using help.\n");
        goto lethe_exec_epilogue;
    }

    cmd = &g_lethe_tool_commands[0];
    cmd_end = cmd + g_lethe_tool_commands_nr;

    while (cmd != cmd_end && strcmp(cmd->ucmd, ucmd) != 0) {
        cmd++;
    }

    if (cmd != cmd_end) {
        has_error = cmd->task();
    } else {
        // TODO(Rafael): Levenshtein distance trinket goes here...
        fprintf(stderr, "ERROR: '%s' is a unknown command. Maybe 'man' command would be more suitable for you, would not?\n",
                        ucmd);
    }

lethe_exec_epilogue:

    return has_error;
}

static int do_drop(void) {
    return 1;
}

static int do_drop_help(void) {
    fprintf(stdout, "use: lethe drop <file name list | glob patterns>\n"
                    "      [--ask-me-nothing |\n"
                    "       --randomizer-lib=<so-filepath>:<func-name>]\n");
    return 0;
}

static int do_help(void) {
    return 1;
}

static int do_help_help(void) {
    return 1;
}

static int do_man(void) {
    return 1;
}

static int do_man_help(void) {
    fprintf(stdout, "use: lethe man\n");
    return 0;
}

static int do_version(void) {
    fprintf(stdout, "lethe-%s\n", g_lethe_tool_version);
    return 0;
}
