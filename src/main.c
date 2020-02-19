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

struct lethe_tool_commands_ctx {
    const char *ucmd;
    int (*task)(void);
};

static int do_drop(void);

static int do_drop_help(void);

static int do_help(void);

static int do_help_help(void); // INFO(Rafael): Children will love this function name...

static int do_man(void);

static int do_man_help(void);

static int do_version(void);

static int lethe_exec(char *(*get_ucmd)(void), void (*null_command)(void),
                      struct lethe_tool_commands_ctx *commands, const size_t commands_nr);

static void null_command(void);

static char *get_help_topic(void);

static void null_help_topic(void);

static struct lethe_tool_commands_ctx g_lethe_tool_commands[] = {
    { "drop",    do_drop     },
    { "help",    do_help     },
    { "man",     do_man      },
    { "version", do_version  }
};

static struct lethe_tool_commands_ctx g_lethe_tool_help_topics[] = {
    { "drop", do_drop_help },
    { "help", do_help_help },
    { "man",  do_man_help  }
};

const char *g_lethe_tool_version = "v1";

int main(int argc, char **argv) {
    lethe_option_set_argc_argv(argc, argv);
    return lethe_exec(lethe_get_ucmd,
                      null_command,
                      &g_lethe_tool_commands[0],
                      sizeof(g_lethe_tool_commands) / sizeof(g_lethe_tool_commands[0]));
}

static int lethe_exec(char *(*get_ucmd)(void), void (*null_command)(void),
                      struct lethe_tool_commands_ctx *commands, const size_t commands_nr) {
    char *ucmd;
    int has_error = 1;
    struct lethe_tool_commands_ctx *cmd, *cmd_end;

    ucmd = get_ucmd();

    if (lethe_get_bool_option("version", 0) || (ucmd != NULL && strcmp(ucmd, "--version") == 0)) {
        // INFO(Rafael): Keeping --version people happy, in silent mode; or keeping those people in silent mode.
        has_error = do_version();
        goto lethe_exec_epilogue;
    }

    if (ucmd == NULL) {
        null_command();
        goto lethe_exec_epilogue;
    }

    cmd = commands;
    cmd_end = cmd + commands_nr;

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

static void null_command(void) {
    fprintf(stderr, "Well, what are intending to do? Try to call me again by using help.\n");
}

static char *get_help_topic(void) {
    return lethe_get_argv(0);
}

static void null_help_topic(void) {
    fprintf(stderr, "Hey human, you need to inform which help topic do you want.\n");
}

static int do_drop(void) {
    return 1;
}

static int do_drop_help(void) {
    fprintf(stdout, "use: lethe drop <file name list | glob patterns>\n"
                    "      [--ask-me-nothing |\n"
                    "       --dym-randomizer=<so-filepath>:<func-name>]\n");
    return 0;
}

static int do_help(void) {
    return lethe_exec(get_help_topic,
                      null_help_topic,
                      &g_lethe_tool_help_topics[0],
                      sizeof(g_lethe_tool_help_topics) / sizeof(g_lethe_tool_help_topics[0]));
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
