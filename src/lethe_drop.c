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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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

static unsigned char *get_rnd_filename(char *filename, lethe_randomizer get_byte);

static int fdoblivion(int fd, const size_t fsize, lethe_randomizer get_byte);

static int lethe_remove(const char *filepath, lethe_randomizer get_byte);

int lethe_do_drop(const char *filepath, const lethe_drop_type dtype, lethe_randomizer get_byte) {
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

    if (stat(filepath, &st) != 0) {
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
            // TODO(Rafael): Do all recursion needed.
        }

        has_error = lethe_remove(filepath, get_byte);
    }

lethe_do_drop_epilogue:

    return has_error;
}


static int fdoblivion(int fd, const size_t fsize, lethe_randomizer get_byte) {
    // TODO(Rafael): Guess what?
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

static unsigned char *get_rnd_filename(char *filename, lethe_randomizer get_byte) {
    char *fp, *fp_end;
    struct stat st;

    fp = filename;
    fp_end = fp + strlen(fp);

    do {
        *fp = g_lethe_allowed_fname_symbols[get_byte() % g_lethe_allowed_fname_symbols_nr];
        fp++;
    } while (fp != fp_end);

    return filename;
}

static int lethe_remove(const char *filepath, lethe_randomizer get_byte) {
    // TODO(Rafael): Unlink stuff.
    return 1;
}
