/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <lethe.h>
#include <lethe_option.h>
#include <lethe_ldist.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#if defined(__unix__)
# include <dlfcn.h>
#elif defined(_WIN32)
# include <windows.h>
#endif

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

static int lethe_exec(char *(*get_ucmd)(void), int (*null_command)(void),
                      struct lethe_tool_commands_ctx *commands, const size_t commands_nr);

static int null_command(void);

static char *get_help_topic(void);

static int help_banner(void);

static void sigint_watchdog(int sig_nr);

static int did_you_mean(const char *ucmd, const int max_distance);

static int has_more(void);

static int has_less(void);

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
    signal(SIGINT | SIGTERM, sigint_watchdog);
    lethe_option_set_argc_argv(argc, argv);
    return lethe_exec(lethe_get_ucmd,
                      null_command,
                      &g_lethe_tool_commands[0],
                      sizeof(g_lethe_tool_commands) / sizeof(g_lethe_tool_commands[0]));
}

static void sigint_watchdog(int sig_nr) {
    // INFO(Rafael): It will be probably the panic button so let's call exit asap.
    fprintf(stdout, "Aborted!\n");
    exit(1);
}

static int lethe_exec(char *(*get_ucmd)(void), int (*null_command)(void),
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
        has_error = null_command();
        goto lethe_exec_epilogue;
    }

    cmd = commands;
    cmd_end = cmd + commands_nr;

    while (cmd != cmd_end && strcmp(cmd->ucmd, ucmd) != 0) {
        cmd++;
    }

    if (cmd != cmd_end) {
        has_error = cmd->task();
    } else if (did_you_mean(ucmd, 2) == 0) {
        fprintf(stderr, "Maybe 'man' command would be more suitable for you, would not?\n");
    }

lethe_exec_epilogue:

    return has_error;
}

static int null_command(void) {
    fprintf(stderr, "Well, what are intending to do? Try to call me again by using help.\n");
    return 1;
}

static char *get_help_topic(void) {
    return lethe_get_argv(0);
}

static int help_banner(void) {
    fprintf(stdout, "lethe is Copyright (C) 2020 by Rafael Santiago.\n\n"
                    "Bug reports, feedback, etc: <voidbrainvoid@tutanota.com> or "
                    "<https://github.com/rafael-santiago/lethe/issues>\n"
                    "_____\n"
                    "usage: lethe <command> [options]\n\n"
                    "*** Are you looking for some quick help topic of a command? Run 'lethe help <command>'.\n"
                    "    Are you new here? Hi there newbie! What about reading some documentation before you screw "
                    "something up? Run 'lethe man' and\n"
                    "    welcome! ;)\n");
    return 0;
}

static int do_drop(void) {
    int has_error = 1;
    int overwrite_nr, rename_nr;
    int a;
    char *arg;
    lethe_drop_type dtype;
    unsigned char (*randomizer)(void) = lethe_default_randomizer;
    char libpath[4096], *func;
#if defined(__unix__)
    void *libdynrandom = NULL;
#elif defined(_WIN32)
    HMODULE libdynrandom = NULL;
#else
# error Some code wanted.
#endif

    if (lethe_set_overwrite_nr(atoi(lethe_get_option("overwrite-passes", "5"))) != 0) {
        fprintf(stderr, "ERROR: Option --overwrite-passes must be a valid number from 1 up to n.\n");
        goto do_drop_epilogue;
    }

    if (lethe_set_rename_nr(atoi(lethe_get_option("rename-passes", "10"))) != 0) {
        fprintf(stderr, "ERROR: Option --rename-passes must be a valid number from 1 up to n.\n");
        goto do_drop_epilogue;
    }

    dtype = kLetheDataOblivion | kLetheFileRemove | kLetheCustomRandomizer;

    if (!lethe_get_bool_option("ask-me-nothing", 0)) {
        dtype |= kLetheUserPrompt;
    }

    if ((arg = lethe_get_option("dyn-randomizer", NULL)) != NULL) {
        if ((func = strstr(arg, ":")) == NULL) {
            fprintf(stderr, "ERROR: Randomizer's function name not informed in --dyn-randomizer option.\n");
            goto do_drop_epilogue;
        }

        memset(libpath, 0, sizeof(libpath));
        memcpy(libpath, arg, func - arg);
        func += 1;

#if defined(__unix__)
        if ((libdynrandom = dlopen(libpath, RTLD_LAZY)) == NULL) {
            fprintf(stderr, "ERROR: Unable to open '%s'.\n", libpath);
            goto do_drop_epilogue;
        }

        if ((randomizer = (unsigned char (*)(void)) dlsym(libdynrandom, func)) == NULL) {
            fprintf(stderr, "ERROR: Randomizer function '%s' not found.\n", func);
            goto do_drop_epilogue;
        }
#elif defined(_WIN32)
        if ((libdynrandom = LoadLibrary(libpath)) == NULL) {
            fprintf(stderr, "ERROR: Unable to open '%s'.\n", libpath);
            goto do_drop_epilogue;
        }

        if ((randomizer = (unsigned char (*)(void)) GetProcAddress(libdynrandom, func)) == NULL) {
            fprintf(stderr, "ERROR: Randomizer function '%s' not found.\n", func);
            goto do_drop_epilogue;
        }
#else
# error Some code wanted.
#endif

    }

    a = 0;

    while ((arg = lethe_get_argv(a++)) != NULL) {
        if (strstr(arg, "--") == arg) {
            continue;
        }
        if (has_error != 0) { // INFO(Rafael): At least one well-succeed drop is considered a succeed dropping process.
            has_error = lethe_drop(arg, dtype, randomizer);
        } else {
            lethe_drop(arg, dtype, randomizer);
        }
    }

do_drop_epilogue:

#if defined(__unix__)
    if (libdynrandom != NULL) {
        dlclose(libdynrandom);
    }
#elif defined(_WIN32)
    if (libdynrandom != NULL) {
        FreeLibrary(libdynrandom);
    }
#else
# error Some code wanted.
#endif

    if (has_error != 0) {
        fprintf(stderr, "No such file or directory.\n");
    }

    return has_error;
}

static int do_drop_help(void) {
    fprintf(stdout, "use: lethe drop\n"
                    "           <file name list | glob patterns>\n"
                    "           [--ask-me-nothing            |\n"
                    "            --overwrite-passes=<number> |\n"
                    "            --rename-passes=<number>    |\n"
                    "            --dyn-randomizer=<so-filepath>:<func-name>]\n");
    return 0;
}

static int do_help(void) {
    return lethe_exec(get_help_topic,
                      help_banner,
                      &g_lethe_tool_help_topics[0],
                      sizeof(g_lethe_tool_help_topics) / sizeof(g_lethe_tool_help_topics[0]));
}

static int do_help_help(void) {
    fprintf(stdout, "use: lethe help <command>\n");
    return 0;
}

static int do_man(void) {
    FILE *pager = stdout, *fp = NULL;
#if defined(__unix__)
    const char *manpath = "/usr/local/share/lethe/doc/MANUAL.txt";
#elif defined(_WIN32)
    const char *manpath = "C:\\lethe\\doc\\MANUAL.txt";
#else
# error Some code wanted.
#endif
    char *content = NULL;
    size_t content_size;
    int has_error = 1;

    if (has_less()) {
        pager = popen("less", "w");
    } else if (has_more()) {
        pager = popen("more", "w");
    }

    if ((fp = fopen(manpath, "rb")) == NULL) {
        fprintf(stderr, "ERROR: Unable to find the manual.\n");
        goto do_man_epilogue;
    }

    fseek(fp, 0L, SEEK_END);
    content_size = (size_t) ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if ((content = (char *) malloc(content_size + 1)) == NULL) {
        fprintf(stderr, "ERROR: Not enough memory.\n");
        goto do_man_epilogue;
    }

    memset(content, 0, content_size + 1);
    fread(content, 1, content_size, fp);

    fprintf(pager, "%s", content);

    has_error = 0;

do_man_epilogue:

    if (pager != NULL && pager != stdout) {
        pclose(pager);
    }

    if (fp != NULL) {
        fclose(fp);
    }

    if (content != NULL) {
        free(content);
    }

    return has_error;
}

static int do_man_help(void) {
    fprintf(stdout, "use: lethe man\n");
    return 0;
}

static int do_version(void) {
    fprintf(stdout, "lethe-%s\n", g_lethe_tool_version);
    return 0;
}

static int did_you_mean(const char *ucmd, const int max_distance) {
    int distances[0xFF];
    size_t d, cmd_nr;
    int has_some_suggestion = 0, s_nr;
    struct lethe_tool_commands_ctx *cmd, *cmd_end;

    for (d = 0; d < sizeof(distances) / sizeof(distances[0]); d++) {
        distances[d] = -1;
    }

    cmd = &g_lethe_tool_commands[0];
    cmd_end = cmd + (cmd_nr = sizeof(g_lethe_tool_commands) / sizeof(g_lethe_tool_commands[0]));

    while (cmd != cmd_end) {
        d = cmd_nr - (cmd_end - cmd);
        distances[d] = levenshtein_distance(ucmd, cmd->ucmd);
        has_some_suggestion |= (distances[d] >= 1 && distances[d] <= max_distance);
        cmd++;
    }

    if (has_some_suggestion) {
        s_nr = 0;
        cmd = &g_lethe_tool_commands[0];
        fprintf(stderr, "Did you mean ");
        while (cmd != cmd_end) {
            d = cmd_nr - (cmd_end - cmd);
            if (distances[d] >= 1 && distances[d] <= max_distance) {
                if (s_nr > 0) {
                    fprintf(stderr, "%s ", ((d + 1) == cmd_nr) ? " or" : ",");
                }
                fprintf(stderr, "'%s'", cmd->ucmd);
                s_nr++;
            }
            cmd++;
        }
        fprintf(stderr, "?\n");
    }

    return has_some_suggestion;
}

static int has_tool(const char *cmd) {
    FILE *p = popen(cmd, "r");
    int has = 0;
    if (p != NULL) {
        has = 1;
        pclose(p);
    }
    return has;
}

static int has_more(void) {
    return has_tool("more --version");
}

static int has_less(void) {
    return has_tool("less --version");
}
