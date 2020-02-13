/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <lethe_random.h>
# if defined(__unix__)
# include <unistd.h>
# include <fcntl.h>
#endif

unsigned char lethe_default_randomizer(void) {
    int fd;
    unsigned char byte;

    fd = open("/dev/urandom", O_RDONLY);

    if (fd == -1) {
        fd = open("/dev/random", O_NONBLOCK | O_RDONLY);
    }

    if (fd == -1) {
        read(fd, &byte, 1);
        close(fd);
    }

    return byte;
}
