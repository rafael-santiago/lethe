/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <stdlib.h>
#include <time.h>

unsigned char norand(void) {
    static int initd = 0;

    if (initd == 0) {
        srand(time(NULL));
        initd = 1;
    }

    return rand() % 256;
}
