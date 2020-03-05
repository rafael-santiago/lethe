/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <lethe_random.h>
# if defined(__unix__)
#  include <unistd.h>
#  include <fcntl.h>
# elif defined(_WIN32)
#  include <windows.h>
#  include <wincrypt.h>
#endif

unsigned char lethe_default_randomizer(void) {
    unsigned char byte;
#if defined(__unix__)
    int fd;

    fd = open("/dev/urandom", O_RDONLY);

    if (fd == -1) {
        fd = open("/dev/random", O_NONBLOCK | O_RDONLY);
    }

    if (fd != -1) {
        read(fd, &byte, 1);
        close(fd);
    }
#elif defined(_WIN32)
    static HCRYPTPROV crypto_ctx = 0;

    if (crypto_ctx == 0 && !CryptAcquireContext(&crypto_ctx, NULL, NULL, PROV_RSA_FULL, 0)) {
        return byte;
    }

    CryptGenRandom(crypto_ctx, 1, &byte);
#else
# error Some code wanted.
#endif

    return byte;
}
