/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <lethe_error.h>
#include <string.h>
#include <stdio.h>

int g_lethe_err;

char g_lethe_filepath[4096];

void lethe_set_error_code(const lethe_error_code code) {
    g_lethe_err = code;
}

void lethe_set_last_filepath(const char *filepath) {
    if (filepath != NULL) {
        snprintf(g_lethe_filepath, sizeof(g_lethe_filepath) - 1, "%s", filepath);
    } else {
        memset(g_lethe_filepath, 0, sizeof(g_lethe_filepath));
    }
}

char *lethe_get_last_error(char *buf, const size_t buf_size) {
    if (buf == NULL) {
        goto lethe_get_last_error_epilogue;
    }

    switch (g_lethe_err) {
        case kLetheErrorNone:
            snprintf(buf, buf_size - 1, "Success.");
            break;

        case kLetheErrorNullFile:
            snprintf(buf, buf_size - 1, "Null file name.");
            break;

        case kLetheErrorNullDropType:
            snprintf(buf, buf_size - 1, "Null drop type.");
            break;

        case kLetheErrorUnableToAccess:
            snprintf(buf, buf_size - 1, "Unable to access file '%s'.", g_lethe_filepath);
            break;

        case kLetheErrorOpenHasFailed:
            snprintf(buf, buf_size - 1, "Unable to open file '%s'.", g_lethe_filepath);
            break;

        case kLetheErrorDataOblivionHasFailed:
            snprintf(buf, buf_size - 1, "Unable to scramble data from file '%s'.", g_lethe_filepath);
            break;

        case kLetheErrorFileRemoveHasFailed:
            snprintf(buf, buf_size - 1, "Unable to remove file '%s'.", g_lethe_filepath);
            break;

        case kLetheErrorNothingToDrop:
            snprintf(buf, buf_size - 1, "Nothing to drop.");
            break;

        default:
            snprintf(buf, buf_size - 1, "You have found a unicorn! Congrats!");
            break;
    }

lethe_get_last_error_epilogue:

    return buf;
}
