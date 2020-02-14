/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <lethe_drop.h>
#include <lethe_error.h>
#include <lethe_random.h>
#include <lethe_strglob.h>
#include <lethe_mkpath.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int g_lethe_drop_rename_nr = 10;

lethe_stat g_lethe_stat = stat;

static char g_lethe_allowed_fname_symbols[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', '_'
};

static size_t g_lethe_allowed_fname_symbols_nr = sizeof(g_lethe_allowed_fname_symbols) /
                                                 sizeof(g_lethe_allowed_fname_symbols[0]);

static unsigned char *get_rnd_databuf(const size_t size, lethe_randomizer get_byte);

static void get_rnd_filename(char *filename, lethe_randomizer get_byte);

static int fdoblivion(int fd, const size_t fsize, lethe_randomizer get_byte);

static int lethe_remove(const char *filepath, lethe_randomizer get_byte);

static int lethe_do_drop(const char *filepath, const lethe_drop_type dtype, lethe_randomizer get_byte);

int lethe_drop_pattern(const char *pattern, const lethe_drop_type dtype, lethe_randomizer get_byte) {
    DIR *dir = NULL;
    int has_error = 1;
    char cwd[4096];
    size_t cwd_size, filename_size;
    struct dirent *dt;
    char fullpath[4096], *filename;
    struct stat st;

    lethe_set_error_last_filepath(".");

    if (getcwd(cwd, sizeof(cwd) - 1) == NULL) {
        lethe_set_error_code(kLetheErrorUnableToAccess);
        goto lethe_drop_pattern_epilogue;
    }

    cwd_size = strlen(cwd);

    lethe_set_error_last_filepath(cwd);

    if ((dir = opendir(cwd)) == NULL) {
        lethe_set_error_code(kLetheErrorUnableToAccess);
        goto lethe_drop_pattern_epilogue;
    }

    has_error = 0;

    while (!has_error && (dt = readdir(dir)) != NULL) {
        filename = dt->d_name;

        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            continue;
        }

        filename_size = strlen(filename);

        if (lethe_strglob(filename, pattern)) {
            lethe_set_error_last_filepath(filename);

            if (lethe_mkpath(fullpath, sizeof(fullpath), cwd, cwd_size, filename, filename_size) == NULL) {
                lethe_set_error_code(kLetheErrorUnableToAccess);
                has_error = 1;
                goto lethe_drop_pattern_epilogue;
            }

            has_error = lethe_do_drop(fullpath, dtype, get_byte);
        }
    }

lethe_drop_pattern_epilogue:

    if (dir != NULL) {
        closedir(dir);
    }

    chdir(cwd);

    return has_error;
}

int lethe_set_rename_nr(const int value) {
    g_lethe_rename_nr = value;
}

void lethe_set_stat(lethe_stat func_addr) {
    g_lethe_stat = func_addr;
}

static int lethe_do_drop(const char *filepath, const lethe_drop_type dtype, lethe_randomizer get_byte) {
    int has_error = 1;
    struct stat st;
    int fd;

    if (filepath == NULL) {
        lethe_set_error_code(kLetheErrorNullFile);
        goto lethe_do_drop_epilogue;
    }

    if ((dtype & kLetheDataOblivion) == 0 &&
        (dtype & kLetheFileRemove)   == 0) {
        lethe_set_error_code(kLetheErrorNullDropType);
        goto lethe_do_drop_epilogue;
    }

    lethe_set_error_last_filepath(filepath);

    if (g_lethe_stat(filepath, &st) != 0) {
        lethe_set_error_code(kLetheErrorUnableToAccess);
        goto lethe_do_drop_epilogue;
    }

    if (get_byte == NULL) {
        get_byte = lethe_default_randomizer;
    }

    if ((dtype & kLetheDataOblivion) && S_ISREG(st.st_mode)) {
        if ((fd = open(filepath, O_WRONLY | O_SYNC)) == -1) {
            lethe_set_error_code(kLetheErrorOpenHasFailed);
            goto lethe_do_drop_epilogue;
        }
        if (fdoblivion(fd, st.st_size, get_byte) != 0) {
            lethe_set_error_code(kLetheErrorDataOblivionHasFailed);
            goto lethe_do_drop_epilogue;
        }
        close(fd);
    }

    if (dtype & kLetheFileRemove) {
        if (S_ISDIR(st.st_mode)) {
            if ((has_error = chdir(filepath)) != 0) {
                goto lethe_do_drop_epilogue;
            }

            // INFO(Rafael): If a directory removing was requested so is inferred that everything within
            //               this directory must be removed.
            if ((has_error = lethe_drop_pattern("*", dtype, get_byte)) != 0) {
                goto lethe_do_drop_epilogue;
            }

            if ((has_error = chdir("..")) != 0) {
                goto lethe_do_drop_epilogue;
            }
        }

        has_error = lethe_remove(filepath, get_byte);
    }

lethe_do_drop_epilogue:

    return has_error;
}

static int fdoblivion(int fd, const size_t fsize, lethe_randomizer get_byte) {
    // WARN(Rafael): This ***is not*** a silver bullet because it depends on the current filesystem (and device) in use.
    //               What optimizations it brings and what heuristics it takes advantage to work on.
    //               Anyway, I am following the basic idea of the DoD standard. Here we do not want to
    //               erase every single trace of the related file. Only its content data is relevant.
    //               Inode infos such as file size, file name and other file metadata are (at first glance)
    //               negligible for an eavesdropper and us either.

    char *buf = NULL;
    int has_error = 1;

    // INFO(Rafael): Bit flipping.

    if ((buf = (unsigned char *) malloc(fsize)) == NULL) {
        goto fdoblivion_epilogue;
    }

    memset(buf, 0xFF, fsize);

    if (write(fd, buf, fsize) != fsize) {
        goto fdoblivion_epilogue;
    }

    fsync(fd);

    memset(buf, 0x00, fsize);

    if (write(fd, buf, fsize) != fsize) {
        goto fdoblivion_epilogue;
    }

    fsync(fd);

#define fdoblivion_paranoid_reverie_step(fd, buf, fsize, get_byte, epilogue) {\
    free(buf);\
    if ((buf = get_rnd_databuf(fsize, get_byte)) == NULL) {\
        goto epilogue;\
    }\
    if (write(fd, buf, fsize) != fsize) {\
        goto epilogue;\
    }\
    fsync(fd);\
}

    // INFO(Rafael): This step of the implemented data wiping is based on the suggestions given by Bruce Schneier
    //               in his book Applied Cryptography [228 pp.].

    fdoblivion_paranoid_reverie_step(fd, buf, fsize, get_byte, fdoblivion_epilogue);
    fdoblivion_paranoid_reverie_step(fd, buf, fsize, get_byte, fdoblivion_epilogue);
    fdoblivion_paranoid_reverie_step(fd, buf, fsize, get_byte, fdoblivion_epilogue);
    fdoblivion_paranoid_reverie_step(fd, buf, fsize, get_byte, fdoblivion_epilogue);
    fdoblivion_paranoid_reverie_step(fd, buf, fsize, get_byte, fdoblivion_epilogue);

#undef fdoblivion_paranoid_reverie_step

    free(buf);
    buf = NULL;

    has_error = 0;

fdoblivion_epilogue:

    if (buf != NULL) {
        free(buf);
    }

    if (has_error) {
        lethe_set_error_code(kLetheErrorDataOblivionHasFailed);
    }

    return has_error;
}

static unsigned char *get_rnd_databuf(const size_t size, lethe_randomizer get_byte) {
    unsigned char *buf = (unsigned char *) malloc(size);
    unsigned char *bp, *bp_end;

    if (buf == NULL) {
        bp = buf;
        bp_end = bp + size;
        while (bp != bp_end) {
            *bp = get_byte();
            bp++;
        }
    }

get_rnd_databuf_epilogue:

    return buf;
}

static void get_rnd_filename(char *filename, lethe_randomizer get_byte) {
    char *fp, *fp_end;
    struct stat st;

    fp = filename;
    fp_end = fp + strlen(fp);

    do {
        *fp = g_lethe_allowed_fname_symbols[get_byte() % g_lethe_allowed_fname_symbols_nr];
        fp++;
    } while (fp != fp_end);
}

static int lethe_remove(const char *filepath, lethe_randomizer get_byte) {
    char ping_pong_paths[2][4096], *curr_fn, *last_fn, *curr_fp, *last_fp;
    char *fn_p[2];
    int has_error = 1, do_nr;
    size_t filepath_size;
    int b;
    struct stat st;

    if (filepath == NULL) {
        goto lethe_remove_epilogue;
    }

    filepath_size = strlen(filepath);

    if (filepath_size > sizeof(ping_pong_paths[0]) - 1) {
        goto lethe_remove_epilogue;
    }

    snprintf(ping_pong_paths[0], sizeof(ping_pong_paths[0]), "%s", filepath);
    snprintf(ping_pong_paths[1], sizeof(ping_pong_paths[1]), "%s", filepath);

    filepath_size -= 1;

#if defined(__unix__)
    while (filepath_size > 0 && filepath[filepath_size] != '/') {
        filepath_size--;
    }
    filepath_size += (filepath_size != 0);
#else
# error Some code wanted.
#endif

    fn_p[0] = &ping_pong_paths[0][filepath_size];
    fn_p[1] = &ping_pong_paths[1][filepath_size];

#define get_curr_fname(curr, b) ( curr = &fn_p[b^0x1][0] )

#define get_curr_fpath(curr, b) ( curr = &ping_pong_paths[b^0x1][0] )
#define get_last_fpath(last, b) ( last = &ping_pong_paths[b    ][0] )

    do_nr = g_lethe_drop_rename_nr;

    b = 0;

    while (do_nr-- > 0) {
        get_last_fpath(last_fp, b);
        get_curr_fpath(curr_fp, b);

        get_curr_fname(curr_fn, b);

        do {
            get_rnd_filename(curr_fn, get_byte);
        } while (g_lethe_stat(curr_fp, &st) == 0);

        if (rename(last_fp, curr_fp) != 0) {
            goto lethe_remove_epilogue;
        }

        b ^= 0x1;
    }

#undef get_curr_fname

#undef get_curr_fpath
#undef get_last_fpath

    has_error = remove(curr_fp);

lethe_remove_epilogue:

    if (has_error) {
        lethe_set_error_code(kLetheErrorFileRemoveHasFailed);
    }

    memset(ping_pong_paths[0], 0, sizeof(ping_pong_paths[0]));
    memset(ping_pong_paths[1], 0, sizeof(ping_pong_paths[0]));

    return has_error;
}
