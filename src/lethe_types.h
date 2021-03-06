/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#ifndef LETHE_LETHE_TYPES_H
#define LETHE_LETHE_TYPES_H 1

#include <lethe_libc.h>

typedef unsigned char (*lethe_randomizer)(void);

typedef enum {
    kLetheDataOblivion     =                         0x1,
    kLetheFileRemove       = kLetheDataOblivion     << 1,
    kLetheCustomRandomizer = kLetheFileRemove       << 1,
    kLetheUserPrompt       = kLetheCustomRandomizer << 1
}lethe_drop_type;

typedef enum {
    kLetheErrorNone = 0,
    kLetheErrorNullFile,
    kLetheErrorNullDropType,
    kLetheErrorUnableToAccess,
    kLetheErrorOpenHasFailed,
    kLetheErrorDataOblivionHasFailed,
    kLetheErrorFileRemoveHasFailed,
    kLetheErrorNothingToDrop,
    kLetheErrorNr
}lethe_error_code;

extern int g_lethe_err;

extern char g_lethe_filepath[4096];

extern int g_lethe_rename_nr;

//extern lethe_stat g_lethe_stat;

#endif
