/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <cutest.h>
#include <lethe_libc.h>
#include <lethe_strglob.h>
#include <lethe_mkpath.h>
#include <lethe_random.h>
#include <lethe_error.h>
#if defined(LETHE_TOOL)
# include <lethe_option.h>
#endif
#include <lethe_drop.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int stat_call_nr = 0;

int randomizer_call_nr = 0;

int memcpy_nr = 0;

int memcmp_nr = 0;

int memset_nr = 0;

static int stat_wrapper(const char *pathname, struct stat *buf);

static unsigned char randomizer_wrapper(void);

static int has_foremost(void);

static int has_grep(void);

#if defined(__unix__)
static int has_found_by_foremost(const char *signature, const size_t signature_size, const char *output_dir);

static int write_foremost_config(const char *header, const size_t header_size, const char *footer, const size_t footer_size, const char *conf);
#endif

#if defined(__unix__)
static int has_found_by_grep(const char *devpath, const char *signature);

static char *get_devpath(void);
#endif

static char *get_random_printable_buffer(const size_t bytes_total);

static int write_data_to_file(const char *filepath, const char *data, size_t data_size);

static char *get_ndata_from_file(const char *filepath, const size_t data_size);

static int lethe(const char *command, const char *user_choices);

static int usr_memcmp(const void *b1, const void *b2, size_t len);

static void *usr_memset(void *b, int c, size_t len);

static void *usr_memcpy(void *dest, const void *src, size_t len);

CUTE_DECLARE_TEST_CASE(lethe_tests_entry);

CUTE_DECLARE_TEST_CASE(lethe_strglob_tests);
CUTE_DECLARE_TEST_CASE(lethe_mkpath_tests);
CUTE_DECLARE_TEST_CASE(lethe_default_randomizer_tests);
CUTE_DECLARE_TEST_CASE(lethe_error_stuff_tests);
#if defined(LETHE_TOOL)
CUTE_DECLARE_TEST_CASE(lethe_option_stuff_tests);
#endif
CUTE_DECLARE_TEST_CASE(lethe_drop_tests);
CUTE_DECLARE_TEST_CASE(lethe_tool_poke_tests);
CUTE_DECLARE_TEST_CASE(lethe_set_memcmp_tests);
CUTE_DECLARE_TEST_CASE(lethe_set_memcpy_tests);
CUTE_DECLARE_TEST_CASE(lethe_set_memset_tests);

CUTE_MAIN(lethe_tests_entry)

CUTE_TEST_CASE(lethe_tests_entry)
    CUTE_RUN_TEST(lethe_set_memcmp_tests);
    CUTE_RUN_TEST(lethe_set_memcpy_tests);
    CUTE_RUN_TEST(lethe_set_memset_tests);
    CUTE_RUN_TEST(lethe_strglob_tests);
    CUTE_RUN_TEST(lethe_mkpath_tests);
    CUTE_RUN_TEST(lethe_default_randomizer_tests);
    CUTE_RUN_TEST(lethe_error_stuff_tests);
#if defined(LETHE_TOOL)
    CUTE_RUN_TEST(lethe_option_stuff_tests);
#endif
    CUTE_RUN_TEST(lethe_drop_tests);
#if defined(LETHE_TOOL)
    CUTE_RUN_TEST(lethe_tool_poke_tests);
#endif
    // INFO(Rafael): Until now lethe_memcmp is available only for future necessities, no calls to memcmp are being done here.
    CUTE_ASSERT(memcmp_nr == 0);
    CUTE_ASSERT(memcpy_nr > 0);
    CUTE_ASSERT(memset_nr > 0);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(lethe_set_memcmp_tests)
    CUTE_ASSERT(lethe_set_memcmp(NULL) != 0);
    CUTE_ASSERT(lethe_set_memcmp(usr_memcmp) == 0);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(lethe_set_memcpy_tests)
    CUTE_ASSERT(lethe_set_memcpy(NULL) != 0);
    CUTE_ASSERT(lethe_set_memcpy(usr_memcpy) == 0);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(lethe_set_memset_tests)
    CUTE_ASSERT(lethe_set_memset(NULL) != 0);
    CUTE_ASSERT(lethe_set_memset(usr_memset) == 0);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(lethe_tool_poke_tests)
    struct stat st;

    CUTE_ASSERT(lethe("help", NULL) == 0);
    CUTE_ASSERT(lethe("help drop", NULL) == 0);
    CUTE_ASSERT(lethe("help man", NULL) == 0);
    CUTE_ASSERT(lethe("help version", NULL) != 0);

#if defined(__unix__)
    CUTE_ASSERT(mkdir("lethe-tool-poking", 0666) == 0);
    CUTE_ASSERT(mkdir("lethe-tool-poking/villains", 0666) == 0);
#elif defined(_WIN32)
    CUTE_ASSERT(mkdir("lethe-tool-poking") == 0);
    CUTE_ASSERT(mkdir("lethe-tool-poking\\villains") == 0);
#else
# error Some code wanted.
#endif

    CUTE_ASSERT(chdir("lethe-tool-poking") == 0);

    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.txt", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.png", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.gif", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.jpg", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.dat", "It does not matter now", 22) == 0);

    CUTE_ASSERT(write_data_to_file("Domesticated-Animals.txt", "Today is the day...", 19) == 0);

    CUTE_ASSERT(write_data_to_file("villains/Fortress.txt", "Your head is like a fortress", 28) == 0);

    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.txt", "Y") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.png", "n") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.gif", "y") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.jpg", "N") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.dat", "y") == 0);

    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.txt", &st) != 0);
    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.png", &st) == 0);
    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.gif", &st) != 0);
    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.jpg", &st) == 0);
    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.dat", &st) != 0);

    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.txt", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.gif", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.dat", "It does not matter now", 22) == 0);

    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.* Domesticated-Animals.txt villains/* --ask-me-nothing", NULL) == 0);

    chdir("..");

    CUTE_ASSERT(lethe("drop lethe-*-poking --ask-me-nothing", NULL) == 0);

    CUTE_ASSERT(stat("lethe-tool-poking", &st) != 0);

    // INFO(Rafael): Now the last test steps will repeated by using a dynamic randomizer.

#if defined(__unix__)
    CUTE_ASSERT(mkdir("lethe-tool-poking", 0666) == 0);
    CUTE_ASSERT(mkdir("lethe-tool-poking/villains", 0666) == 0);
#elif defined(_WIN32)
    CUTE_ASSERT(mkdir("lethe-tool-poking") == 0);
    CUTE_ASSERT(mkdir("lethe-tool-poking\\villains") == 0);
#else
# error Some code wanted.
#endif

    CUTE_ASSERT(chdir("lethe-tool-poking") == 0);

    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.txt", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.png", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.gif", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.jpg", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.dat", "It does not matter now", 22) == 0);

    CUTE_ASSERT(write_data_to_file("Domesticated-Animals.txt", "Today is the day...", 19) == 0);

    CUTE_ASSERT(write_data_to_file("villains/Fortress.txt", "Your head is like a fortress", 28) == 0);

#if defined(__unix__)
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.txt --dyn-randomizer=../dynrand/lib/dynrand1.so:norand", "Y") != 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.txt --dyn-randomizer=../dynrand/lib/dynrand.so:norandom", "Y") != 0);
#elif defined(_WIN32)
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.txt --dyn-randomizer=../dynrand/lib/dynrand1.dll:norand", "Y") != 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.txt --dyn-randomizer=../dynrand/lib/dynrand.dll:norandom", "Y") != 0);
#else
# error Some code wanted.
#endif

#if defined(__unix__)
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.txt --dyn-randomizer=../dynrand/lib/dynrand.so:norand", "Y") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.png --dyn-randomizer=../dynrand/lib/dynrand.so:norand", "n") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.gif --dyn-randomizer=../dynrand/lib/dynrand.so:norand", "y") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.jpg --dyn-randomizer=../dynrand/lib/dynrand.so:norand", "N") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.dat --dyn-randomizer=../dynrand/lib/dynrand.so:norand", "y") == 0);
#elif defined(_WIN32)
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.txt --dyn-randomizer=..\\dynrand\\lib\\dynrand.dll:norand", "Y") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.png --dyn-randomizer=..\\dynrand\\lib\\dynrand.dll:norand", "n") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.gif --dyn-randomizer=..\\dynrand\\lib\\dynrand.dll:norand", "y") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.jpg --dyn-randomizer=..\\dynrand\\lib\\dynrand.dll:norand", "N") == 0);
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.dat --dyn-randomizer=..\\dynrand\\lib\\dynrand.dll:norand", "y") == 0);
#else
# error Some code wanted.
#endif

    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.txt", &st) != 0);
    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.png", &st) == 0);
    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.gif", &st) != 0);
    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.jpg", &st) == 0);
    CUTE_ASSERT(stat("The-Way-You-Used-To-Do.dat", &st) != 0);

    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.txt", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.gif", "It does not matter now", 22) == 0);
    CUTE_ASSERT(write_data_to_file("The-Way-You-Used-To-Do.dat", "It does not matter now", 22) == 0);

#if defined(__unix__)
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.* Domesticated-Animals.txt villains/* --ask-me-nothing"
                      " --dyn-randomizer=../dynrand/lib/dynrand.so:norand", NULL) == 0);
#elif defined(_WIN32)
    CUTE_ASSERT(lethe("drop The-Way-You-Used-To-Do.* Domesticated-Animals.txt villains/* --ask-me-nothing"
                      " --dyn-randomizer=..\\dynrand\\lib\\dynrand.dll:norand", NULL) == 0);
#else
# error Some code wanted.
#endif

    chdir("..");

#if defined(__unix__)
    CUTE_ASSERT(lethe("drop lethe-*-poking --ask-me-nothing --dyn-randomizer=dynrand/lib/dynrand.so:norand", NULL) == 0);
#elif defined(_WIN32)
    CUTE_ASSERT(lethe("drop lethe-*-poking --ask-me-nothing --dyn-randomizer=dynrand/lib/dynrand.dll:norand", NULL) == 0);
#else
# error Some code wanted.
#endif

    CUTE_ASSERT(stat("lethe-tool-poking", &st) != 0);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(lethe_drop_tests)
    char *buf, *temp;
    struct stat st;
    char cmdline[4096];
    size_t buf_size;

    CUTE_ASSERT(lethe_set_stat(NULL) != 0);
    CUTE_ASSERT(lethe_set_stat(stat_wrapper) == 0);
    CUTE_ASSERT(lethe_set_rename_nr(0) != 0);
    CUTE_ASSERT(lethe_set_overwrite_nr(-1) != 0);
    CUTE_ASSERT(lethe_set_overwrite_nr(10203102) == 0);
    CUTE_ASSERT(lethe_set_overwrite_nr(1) == 0);
    system("rm -rf lethe-lab");
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // WARN(Rafael): At this point do not remove the chdir stuff otherwise you can lose important files.!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if defined(__unix__)
    CUTE_ASSERT(mkdir("lethe-lab", 0666) == 0);
#elif defined(_WIN32)
    CUTE_ASSERT(mkdir("lethe-lab") == 0);
#else
# error Some code wanted.
#endif
    CUTE_ASSERT(chdir("lethe-lab") == 0);
    buf = get_random_printable_buffer(10);
    CUTE_ASSERT(write_data_to_file("data.txt", buf, 10) == 0);
    CUTE_ASSERT(lethe_drop("pata.txt", kLetheDataOblivion) != 0);
    CUTE_ASSERT(lethe_drop("data.txt", kLetheDataOblivion) == 0);
    CUTE_ASSERT(stat("data.txt", &st) == 0);
    temp = get_ndata_from_file("data.txt", 10);
    CUTE_ASSERT(temp != NULL);
    CUTE_ASSERT(memcmp(temp, buf, 10) != 0);
    free(buf);
    free(temp);
    CUTE_ASSERT(remove("data.txt") == 0);
    buf = get_random_printable_buffer(10);
    CUTE_ASSERT(write_data_to_file("data.txt", buf, 10) == 0);
    free(buf);
    CUTE_ASSERT(stat("data.txt", &st) == 0);
    CUTE_ASSERT(lethe_drop("data.txt", kLetheDataOblivion | kLetheFileRemove) == 0);
    CUTE_ASSERT(stat("data.txt", &st) != 0);
    buf = get_random_printable_buffer(10);
    CUTE_ASSERT(write_data_to_file("data.txt", buf, 10) == 0);
    free(buf);
    CUTE_ASSERT(write_data_to_file("data.txt", buf, 10) == 0);
    CUTE_ASSERT(stat("data.txt", &st) == 0);
#if defined(__unix__)
    CUTE_ASSERT(mkdir("sub-dir", 0666) == 0);
#elif defined(_WIN32)
    CUTE_ASSERT(mkdir("sub-dir") == 0);
#else
# error Some code wanted.
#endif
    CUTE_ASSERT(write_data_to_file("sub-dir/do-not-forget-the-joker.txt", buf, 10) == 0);
#if defined(__unix__)
    CUTE_ASSERT(mkdir("sub-dir/empty", 0666) == 0);
#elif defined(_WIN32)
    CUTE_ASSERT(mkdir("sub-dir/empty") == 0);
#else
# error Some code wanted.
#endif
    CUTE_ASSERT(chdir("..") == 0);
    CUTE_ASSERT(lethe_drop("lethe-la[b]", kLetheDataOblivion | kLetheFileRemove | kLetheCustomRandomizer,
                           randomizer_wrapper) == 0);
    CUTE_ASSERT(stat("lethe-lab", &st) != 0);
    CUTE_ASSERT(stat_call_nr > 0);
    CUTE_ASSERT(randomizer_call_nr > 0);

    if (CUTE_GET_OPTION("quick-tests") == NULL) {
        // INFO(Rafael): Now we will test it against foremost if possible.

        if (has_foremost()) {
#if defined(__unix__)
            system("rm -rf recovery");
            fprintf(stdout, "INFO: Nice, you have Foremost installed. Let's check if it can caught files removed by Lethe.\n"
                            "      Firstly, we will generate some control data and try to find it with Foremost.\n");

            buf_size = 100010;

            buf = get_random_printable_buffer(buf_size);
            CUTE_ASSERT(write_data_to_file("data.txt", buf, buf_size) == 0);
            sleep(15);
            CUTE_ASSERT(system("sync data.txt") == 0);
            sleep(15);

            CUTE_ASSERT(write_foremost_config(buf, 10, buf + buf_size - 10, 10, "foremost.conf") == 0);
            CUTE_ASSERT(system("rm -f data.txt") == 0);
            fprintf(stdout, "      Control data was removed. Now trying to recover it with Foremost... Hold on...\n");

            // INFO(Rafael): This is the control data. Foremost must be able to recover this piece of information.
            snprintf(cmdline, sizeof(cmdline) - 1, "foremost -i %s -c foremost.conf -o recovery", get_devpath());
            CUTE_ASSERT(system(cmdline) == 0);

            // INFO(Rafael): Since the search is being done over an on-line device, depending on the disk activity
            //               the entire file will not be actually recovered. Due to it, a half file recovery will be considered
            //               a successfuly file recovering.

            CUTE_ASSERT(has_found_by_foremost(buf, buf_size >> 1, "recovery") == 1);

            fprintf(stdout, "INFO: Nice, control data was actually found by Foremost.\n");

            CUTE_ASSERT(system("rm -rf recovery") == 0);
            CUTE_ASSERT(remove("foremost.conf") == 0);

            free(buf);

            fprintf(stdout, "      Now we will generate a random piece of data, remove it by using Lethe and try to recover\n"
                            "      it with Foremost. Wait...\n");

            buf_size = 100010;
            buf = get_random_printable_buffer(buf_size);
            CUTE_ASSERT(write_data_to_file("data.txt", buf, buf_size) == 0);
            sleep(15);
            CUTE_ASSERT(system("sync data.txt") == 0);
            sleep(15);

            CUTE_ASSERT(write_foremost_config(buf, 10, buf + buf_size - 10, 10, "foremost.conf") == 0);

            CUTE_ASSERT(lethe_drop("data.txt", kLetheDataOblivion | kLetheFileRemove) == 0);

            fprintf(stdout, "      Test data was removed by using Lethe. Now trying to recover it with Foremost...\n"
                            "      Hold on...\n");

            // INFO(Rafael): This is the test data. Foremost must not be able to recover this piece of information.
            snprintf(cmdline, sizeof(cmdline) - 1, "foremost -i %s -c foremost.conf -o recovery", get_devpath());
            CUTE_ASSERT(system(cmdline) == 0);

            CUTE_ASSERT(has_found_by_foremost(buf, buf_size >> 1, "recovery") == 0);

            fprintf(stdout, "INFO: Everything looks fine on your system!\n"
                            "      Foremost could not recover data removed by Lethe ;)\n");

            CUTE_ASSERT(system("rm -rf recovery") == 0);
            CUTE_ASSERT(remove("foremost.conf") == 0);

            free(buf);

            fprintf(stdout, "\nINFO: Now let's only test data oblivion. File will stay but its content will be forgotten.\n");

            buf_size = 100010;
            buf = get_random_printable_buffer(buf_size);
            CUTE_ASSERT(write_data_to_file("data.txt", buf, buf_size) == 0);
            sleep(15);
            CUTE_ASSERT(system("sync data.txt") == 0);
            sleep(15);

            fprintf(stdout, "      Test data created... Now let's forget this content by using Lethe... Wait...\n");

            CUTE_ASSERT(write_foremost_config(buf, 10, buf + buf_size - 10, 10, "foremost.conf") == 0);

            CUTE_ASSERT(lethe_drop("data.txt", kLetheDataOblivion) == 0);

            CUTE_ASSERT(stat("data.txt", &st) == 0);

            fprintf(stdout, "      Done. Now trying to recover it with Foremost... Hold on...\n");

            snprintf(cmdline, sizeof(cmdline) - 1, "foremost -i %s -c foremost.conf -o recovery", get_devpath());
            CUTE_ASSERT(system(cmdline) == 0);

            CUTE_ASSERT(has_found_by_foremost(buf, buf_size, "recovery") == 0);
            CUTE_ASSERT(has_found_by_foremost(buf, buf_size >> 1, "recovery") == 0);

            fprintf(stdout, "INFO: Nice! Foremost could not recover data forgotten with Lethe ;)\n");

            CUTE_ASSERT(remove("data.txt") == 0);
            CUTE_ASSERT(system("rm -rf recovery") == 0);
            CUTE_ASSERT(remove("foremost.conf") == 0);

            free(buf);
#endif
        } else {
            if (has_grep()) {
#if defined(__unix__)
                fprintf(stdout, "INFO: Nice, you have cat, strings and grep installed. Let's check if them can caught files removed by Lethe.\n"
                                "      Firstly, we will generate some control data and try to find it with these set of tools.\n");

                buf_size = 20;

                buf = get_random_printable_buffer(buf_size);
                CUTE_ASSERT(write_data_to_file("data.txt", buf, buf_size) == 0);
                sleep(15);
                CUTE_ASSERT(system("sync data.txt") == 0);
                sleep(15);

                CUTE_ASSERT(system("rm -f data.txt") == 0);
                fprintf(stdout, "      Control data was removed. Now trying to recover it with grep and stuff... Hold on...\n");


                CUTE_ASSERT(has_found_by_grep(get_devpath(), buf) == 1);

                fprintf(stdout, "INFO: Nice, control data was actually found by grep.\n");

                free(buf);

                fprintf(stdout, "      Now we will generate a random piece of data, remove it by using Lethe and try to recover\n"
                                "      it with grep and stuff. Wait...\n");

                buf_size = 20;
                buf = get_random_printable_buffer(buf_size);
                CUTE_ASSERT(write_data_to_file("data.txt", buf, buf_size) == 0);
                sleep(15);
                CUTE_ASSERT(system("sync data.txt") == 0);
                sleep(15);

                CUTE_ASSERT(lethe_drop("data.txt", kLetheDataOblivion | kLetheFileRemove) == 0);

                fprintf(stdout, "      Test data was removed by using Lethe. Now trying to recover it with grep and stuff...\n"
                                "      Hold on...\n");

                CUTE_ASSERT(has_found_by_grep(get_devpath(), buf) == 0);

                fprintf(stdout, "INFO: Everything looks fine on your system!\n"
                                "      It could not recover data removed by Lethe ;)\n");

                free(buf);

                fprintf(stdout, "\nINFO: Now let's only test data oblivion. File will stay but its content will be forgotten.\n");

                buf_size = 20;
                buf = get_random_printable_buffer(buf_size);
                CUTE_ASSERT(write_data_to_file("data.txt", buf, buf_size) == 0);
                sleep(15);
                if (system("fsync data.txt") != 0) {
                    CUTE_ASSERT(system("sync data.txt") == 0);
                }
                sleep(15);

                fprintf(stdout, "      Test data created... Now let's forget this content by using Lethe... Wait...\n");

                CUTE_ASSERT(lethe_drop("data.txt", kLetheDataOblivion) == 0);

                CUTE_ASSERT(stat("data.txt", &st) == 0);

                fprintf(stdout, "      Done. Now trying to recover it with grep and stuff... Hold on...\n");

                CUTE_ASSERT(has_found_by_grep(get_devpath(), buf) == 0);

                fprintf(stdout, "INFO: Nice! Grep and stuff could not recover data forgotten with Lethe ;)\n");

                CUTE_ASSERT(remove("data.txt") == 0);
                free(buf);
#endif
            } else {
#if defined(__unix__)
                fprintf(stdout, "WARN: Unfortunately, this test cannot really ensure if the implemented data wiping is\n"
                                "      actually working on your system. For doing it you need to install 'Foremost' data\n"
                                "      recovery tool.\n");
#else
                fprintf(stdout, "WARN: Unfortunately, this test cannot really ensure if the implemented data wiping is\n"
                                "      actually working on your system. Until now, Lethe needs Foremost for doing it, thus\n"
                                "      it is only available on Unix-like environments.\n");
#endif
            }
        }
    } else {
        fprintf(stdout, "WARN: Some test steps were skipped here.\n");
    }
CUTE_TEST_CASE_END

#if defined(LETHE_TOOL)
CUTE_TEST_CASE(lethe_option_stuff_tests)
    char *argv[] = {
        "./lethe",
        "test",
        "--foo=bar",
        "--bar=foo",
        "--foobar"
    };
    int argc = 5, a;
    struct option_tests {
        int bool;
        char *option;
        char *default_s_value, *expected_s_value;
        int default_b_value, expected_b_value;
    } test_vector[] = {
        { 0, "foo", "foo", "bar", 0, 0 },
        { 0, "bar", "bar", "foo", 0, 0 },
        { 1, "foobar", NULL, NULL, 0, 1 },
        { 1, "barfoo", NULL, NULL, 1, 1 },
        { 0, "Zzzz", "Rooc-fiu...", "Rooc-fiu...", 0, 0 }
    }, *test, *test_end;
    char *data;

    CUTE_ASSERT((data = lethe_get_option("boom", "Muhauhahuahauha!!!")) != NULL);
    CUTE_ASSERT(strcmp(data, "Muhauhahuahauha!!!") == 0);

    CUTE_ASSERT(lethe_get_bool_option("boom", 0) == 0);

    lethe_option_set_argc_argv(0, argv);

    CUTE_ASSERT(lethe_get_ucmd() == NULL);
    CUTE_ASSERT(lethe_get_argv(0) == NULL);

    lethe_option_set_argc_argv(1, argv);

    CUTE_ASSERT(lethe_get_ucmd() == NULL);
    CUTE_ASSERT(lethe_get_argv(0) == NULL);

    lethe_option_set_argc_argv(2, argv);

    data = lethe_get_ucmd();
    CUTE_ASSERT(data != NULL);
    CUTE_ASSERT(strcmp(data, "test") == 0);
    CUTE_ASSERT(lethe_get_argv(0) == NULL);
    CUTE_ASSERT(lethe_get_argv(1010101) == NULL);

    lethe_option_set_argc_argv(argc, argv);

    CUTE_ASSERT(lethe_get_bool_option("test", 0) == 0);

    data = lethe_get_ucmd();
    CUTE_ASSERT(strcmp(data, "test") == 0);

    test = &test_vector[0];
    test_end = test + sizeof(test_vector) / sizeof(test_vector[0]);

    while (test != test_end) {
        if (test->bool) {
            CUTE_ASSERT(lethe_get_bool_option(test->option, test->default_b_value) == test->expected_b_value);
        } else {
            CUTE_ASSERT((data = lethe_get_option(test->option, test->default_s_value)) != NULL);
            CUTE_ASSERT(strcmp(data, test->expected_s_value) == 0);
        }
        test++;
    }

    for (a = 2; a < argc; a++) {
        data = lethe_get_argv(a - 2);
        CUTE_ASSERT(data != NULL);
        CUTE_ASSERT(strcmp(data, argv[a]) == 0);
    }

    for (a = argc; a < 256; a++) {
        CUTE_ASSERT(lethe_get_argv(a) == NULL);
    }
CUTE_TEST_CASE_END
#endif

CUTE_TEST_CASE(lethe_error_stuff_tests)
    struct lethe_error_caiau_ctx {
        int error_code;
        char *filepath;
        char *expected_message;
    } test_vector[] = {
        { kLetheErrorNone, "At/the/helm/of/hells/ships", "Success." }, // ;P
        { kLetheErrorNullFile, "", "Null file name." },
        { kLetheErrorNullDropType, "", "Null drop type." },
        { kLetheErrorUnableToAccess, "your/butt/butt-head/he/he/he", "Unable to access file 'your/butt/butt-head/he/he/he'." },
        { kLetheErrorOpenHasFailed, "Beavis.uhhhh.'mouth'.hu-hu-hu!", "Unable to open file 'Beavis.uhhhh.'mouth'.hu-hu-hu!'." },
        { kLetheErrorDataOblivionHasFailed, "Eggs.Duh", "Unable to scramble data from file 'Eggs.Duh'." },
        { kLetheErrorFileRemoveHasFailed, "Caneta_Azul", "Unable to remove file 'Caneta_Azul'." }, // ROM 'cached'.
        { kLetheErrorNothingToDrop, "", "Nothing to drop." },
        { kLetheErrorNr, "Oh! yeah! hehehehe huhhhuhu Destroy! Destroy! Destroy!!! hehehehe hhuhuhuh",
                         "You have found a unicorn! Congrats!" } // I am a crazy person, just ignore.
    }, *test, *test_end;
    char buf[1024];

    test = &test_vector[0];
    test_end = test + sizeof(test_vector) / sizeof(test_vector[0]);

    while (test != test_end) {
        lethe_set_error_code(test->error_code);
        lethe_set_last_filepath(test->filepath);
        CUTE_ASSERT(lethe_get_last_error(NULL, sizeof(buf)) == NULL);
        CUTE_ASSERT(lethe_get_last_error(buf, sizeof(buf)) == &buf[0]);
        CUTE_ASSERT(strcmp(buf, test->expected_message) == 0);
        lethe_set_last_filepath(NULL);
        CUTE_ASSERT(lethe_get_last_error(buf, sizeof(buf)) == &buf[0]); // INFO(Rafael): It must not explode at our faces.
        test++;
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(lethe_default_randomizer_tests)
    int p;
    // INFO(Rafael): It cannot block. If you are using a well-tested build of your OS it wont! ;)
    for (p = 0; p < 1000; p++) {
        lethe_default_randomizer();
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(lethe_mkpath_tests)
    struct mkpath_tests_ctx {
        const char *a;
        const char *b;
        const char *expected;
    } test_vector[] = {
#if defined(__unix__)
        { "Raja", "Haje", "Raja/Haje" },
        { "Raja/", "Haje", "Raja/Haje" },
        { "Raja" , "/Haje", "Raja/Haje" },
        { "Raja/", "/Haje", "Raja/Haje" },
        { "Raja/////////", "//////////Haje","Raja/Haje"},
        { "Raja////////", "Haje", "Raja/Haje"},
        { "Raja", "///////////////////////////////////////////////Haje/", "Raja/Haje/" },
        { "/Raja", "Haje", "/Raja/Haje" },
        { "", "Haje", "Haje" },
        { "Raja", "", "Raja" },
        { "/", "/", "/" },
#elif defined(_WIN32)
        { "Raja", "Haje", "Raja\\Haje" },
        { "Raja/", "Haje", "Raja\\Haje" },
        { "Raja" , "/Haje", "Raja\\Haje" },
        { "Raja/", "/Haje", "Raja\\Haje" },
        { "Raja/////////", "//////////Haje","Raja\\Haje"},
        { "Raja////////", "Haje", "Raja\\Haje"},
        { "Raja", "///////////////////////////////////////////////Haje/", "Raja\\Haje/" },
        { "/Raja", "Haje", "/Raja\\Haje" },
        { "", "Haje", "Haje" },
        { "Raja", "", "Raja" },
        { "/", "/", "/" },
        { "Raja\\", "Haje", "Raja\\Haje" },
        { "Raja" , "/Haje", "Raja\\Haje" },
        { "Raja\\", "/Haje", "Raja\\Haje" },
        { "Raja\\\\\\\\\\\\\\\\\\", "\\\\\\\\\\\\\\\\\\\\Haje","Raja\\Haje"},
        { "Raja\\\\\\\\\\\\\\\\", "Haje", "Raja\\Haje"},
        { "Raja", "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\Haje\\", "Raja\\Haje\\" },
        { "C:\\Raja", "Haje", "C:\\Raja\\Haje" },
        { "", "Haje", "Haje" },
        { "Raja", "", "Raja" },
        { "\\", "\\", "\\" },
#else
# error Some code wanted.
#endif
        { "", "", "" }
    }, *test, *test_end;
    char tiny_buf[1], buf[4096];
    size_t a_size, b_size;

    test = &test_vector[0];
    test_end = test + sizeof(test_vector) / sizeof(test_vector[0]);

    CUTE_ASSERT(lethe_mkpath(NULL, sizeof(buf), test->a, 4, test->b, 4) == NULL);

    CUTE_ASSERT(lethe_mkpath(buf, 0, test->a, 4, test->b, 4) == NULL);

    CUTE_ASSERT(lethe_mkpath(buf, sizeof(buf), NULL, 4, test->b, 4) == NULL);

    CUTE_ASSERT(lethe_mkpath(buf, sizeof(buf), test->a, 4, NULL, 4) == NULL);

    CUTE_ASSERT(lethe_mkpath(buf, sizeof(buf), test->a, sizeof(buf), test->b, 4) == NULL);

    CUTE_ASSERT(lethe_mkpath(buf, sizeof(buf), test->a, 4, test->b, sizeof(buf)) == NULL);

    while (test != test_end) {
        memset(buf, 0, sizeof(buf));
        a_size = strlen(test->a);
        b_size = strlen(test->b);
        CUTE_ASSERT(lethe_mkpath(tiny_buf, sizeof(tiny_buf), test->a, a_size, test->b, b_size) == NULL);
        CUTE_ASSERT(lethe_mkpath(buf, sizeof(buf), test->a, a_size, test->b, b_size) == &buf[0]);
        CUTE_ASSERT(strcmp(buf, test->expected) == 0);
        test++;
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(lethe_strglob_tests)
    struct strglob_tests_ctx {
        const char *str;
        const char *pattern;
        int result;
    };
    struct strglob_tests_ctx tests[] = {
        { NULL,                         NULL                                                       , 0 },
        { "abc",                        "abc"                                                      , 1 },
        { "abc",                        "ab"                                                       , 0 },
        { "abc",                        "a?c"                                                      , 1 },
        { "abc",                        "ab[abdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.c]", 1 },
        { "abc",                        "ab[abdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.?]", 0 },
        { "ab*",                        "ab[c*]"                                                   , 1 },
        { "ab*",                        "ab[*c]"                                                   , 1 },
        { "abc",                        "ab*"                                                      , 1 },
        { "abc",                        "abc*"                                                     , 1 },
        { "strglob.c",                  "strglo*.c"                                                , 1 },
        { "parangaricutirimirruaru!!!", "*"                                                        , 1 },
        { "parangaritititero",          "?"                                                        , 0 },
        { "parangaritititero",          "?*"                                                       , 1 },
        { "parangaricutirimirruaru",    "paran*"                                                   , 1 },
        { "parangaricutirimirruaru",    "parruari"                                                 , 0 },
        { "parangaricutirimirruaru",    "paran*garicuti"                                           , 0 },
        { "parangaricutirimirruaru",    "paran*garicutirimirruaru"                                 , 1 },
        { "parangaricutirimirruaru",    "paran*ru"                                                 , 1 },
        { "hell yeah!",                 "*yeah!"                                                   , 1 },
        { ".",                          "*[Gg]lenda*"                                              , 0 },
    };
    size_t tests_nr = sizeof(tests) / sizeof(tests[0]), t;

    for (t = 0; t < tests_nr; t++) {
        CUTE_ASSERT(lethe_strglob(tests[t].str, tests[t].pattern) == tests[t].result);
    }
CUTE_TEST_CASE_END

static int has_foremost(void) {
#if defined(__unix__)
    return (system("foremost -V > /dev/null") == 0);
#else
    return 0;
#endif
}

#if defined(__unix__)
static int has_found_by_foremost(const char *signature, const size_t signature_size, const char *output_dir) {
    DIR *dir = opendir(output_dir);
    int has_found = 0;
    struct dirent *dt;
    char *filename;
    struct stat st;
    FILE *fp;
    const char *sp, *sp_end;
    char *data, *d, *d_end, *ld;
    size_t data_size;
    char cwd[4096];

    if (dir == NULL) {
        return 0;
    }

    getcwd(cwd, sizeof(cwd));

    stat(output_dir, &st);

    if (S_ISDIR(st.st_mode) && chdir(output_dir) != 0) {
        goto has_found_by_foremost_epilogue;
    }

    while ((dt = readdir(dir)) != NULL && !has_found) {
        filename = &dt->d_name[0];
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            continue;
        }
        if (stat(filename, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            has_found = has_found_by_foremost(signature, signature_size, filename);
        } else if (S_ISREG(st.st_mode)) {
            if ((fp = fopen(filename, "rb")) == NULL) {
                printf("ERROR: Unable to open '%s'.\n", filename);
                continue;
            }
            fseek(fp, 0L, SEEK_END);
            data_size = ftell(fp);
            fseek(fp, 0L, SEEK_SET);
            if ((data = (char *) malloc(data_size)) != NULL) {
                fread(data, 1, data_size, fp);
                d = data;
                d_end = d + data_size;
                while (!has_found && d != d_end) {
                    sp = signature;
                    sp_end = sp + signature_size;
                    has_found = 1;
                    ld = d;
                    while (has_found && sp != sp_end && d != d_end) {
                        has_found = (*sp == *d);
                        sp++;
                        d++;
                    }
                    d = ld + 1;
                }
                free(data);
            }
            fclose(fp);
        }
    }

has_found_by_foremost_epilogue:

    closedir(dir);

    chdir(cwd);

    return has_found;
}

static int write_foremost_config(const char *header, const size_t header_size, const char *footer, const size_t footer_size, const char *conf) {
    FILE *fp;
    const char *d, *d_end;

    if ((fp = fopen(conf, "w")) == NULL) {
        return 1;
    }

    fprintf(fp, "txt\ty\t1000000\t");

    d = header;
    d_end = d + header_size;

    while (d != d_end) {
        fprintf(fp, "\\x%.2X", *d);
        d++;
    }

    fprintf(fp, "\t");

    d = footer;
    d_end = d + footer_size;

    while (d != d_end) {
        fprintf(fp, "\\x%.2X", *d);
        d++;
    }

    fprintf(fp, "\tASCII\n");

    fclose(fp);

    return 0;
}
#endif

static int stat_wrapper(const char *pathname, struct stat *buf) {
    stat_call_nr++;
    return stat(pathname, buf);
}

static unsigned char randomizer_wrapper(void) {
    randomizer_call_nr++;
    return lethe_default_randomizer();
}

static char *get_random_printable_buffer(const size_t bytes_total) {
    static char bytes[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
                            't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
                            'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
    const size_t bytes_nr = sizeof(bytes) / sizeof(bytes[0]);
    char *buf = (char *) malloc(bytes_total), *bp, *bp_end;
    if (buf == NULL) {
        return NULL;
    }
    bp = buf;
    bp_end = bp + bytes_total;
    while (bp != bp_end) {
        *bp = bytes[lethe_default_randomizer() % bytes_nr];
        bp++;
    }
    return buf;
}

static int write_data_to_file(const char *filepath, const char *data, size_t data_size) {
    FILE *fp;
    if ((fp = fopen(filepath, "wb")) == NULL) {
        return 1;
    }
    fprintf(fp, "\n");
    fwrite(data, 1, data_size, fp);
    fprintf(fp, "\n");
    fclose(fp);
    return 0;
}

static char *get_ndata_from_file(const char *filepath, const size_t data_size) {
    FILE *fp;
    char *data;
    if ((fp = fopen(filepath, "rb")) == NULL) {
        return NULL;
    }
    if ((data = (char *)malloc(data_size)) == NULL) {
        return NULL;
    }
    fread(data, 1, data_size, fp);
    fclose(fp);
    return data;
}

static int lethe(const char *command, const char *user_choices) {
    FILE *fp;
#if defined(__unix__)
    const char *binary_path = "bin/lethe";
#elif defined(_WIN32)
    const char *binary_path = "bin\\lethe.exe";
#else
# error Some code wanted.
#endif
    char *usr_inputs = "";
    char execline[4096];
    char indirections[4096];
    int exit_code;
    struct stat st;

    if (user_choices != NULL) {
        if ((fp = fopen(".lethe_usr_inputs", "wb")) == NULL) {
            fprintf(stderr, "Unable to create '.lethe_usr_inputs'.\n");
            return 1;
        }
        fprintf(fp, "%s", user_choices);
        fclose(fp);
        usr_inputs = " < .lethe_usr_inputs";
    }

    memset(indirections, 0, sizeof(indirections));

#if defined(__unix__)
    do {
        strcat(indirections, "../");
        snprintf(execline, sizeof(execline) - 1, "%sbin/lethe", indirections);
    } while (stat(execline, &st) != 0);
#elif defined(_WIN32)
    do {
        strcat(indirections, "..\\");
        snprintf(execline, sizeof(execline) - 1, "%sbin\\lethe.exe", indirections);
    } while (stat(execline, &st) != 0);
#else
# error Some code wanted.
#endif

    snprintf(execline, sizeof(execline) - 1, "%s%s %s %s", indirections, binary_path, command, usr_inputs);

    exit_code = system(execline);

    remove(".lethe_usr_inputs");

    return exit_code;
}

#if defined(__unix__)
static int has_found_by_grep(const char *devpath, const char *signature) {
    char cmdline[4096];
    snprintf(cmdline, sizeof(cmdline), "cat %s | strings | grep %s --max-count=1", devpath, signature);
    return (system(cmdline) == 0);
}
#endif

static int has_grep(void) {
#if defined(__unix__)
    return (system("cat /dev/null > /dev/null")     == 0 &&
            system("which strings > /dev/null") == 0 &&
            system("grep --version > /dev/null")    == 0);
#elif defined(_WIN32)
    return 0;
#else
# error Some code wanted.
#endif
}

#if defined(__unix__)
static char *get_devpath(void) {
    static char devpath[4096] = "", *d;
    char indirections[4096] = "";
    struct stat st;
    FILE *fp;
    size_t devpath_size;

    do {
        strcat(indirections, "../");
        snprintf(devpath, sizeof(devpath) - 1, "%sDEV_PATH", indirections);
    } while (stat(devpath, &st) != 0);
    if ((fp = fopen(devpath, "rb")) != NULL) {
        fseek(fp, 0L, SEEK_END);
        devpath_size = (size_t) ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        memset(devpath, 0, sizeof(devpath));
        fread(devpath, 1, devpath_size, fp);
        if ((d = strstr(devpath, "\n")) != NULL) {
            *d = 0;
        }
        fclose(fp);
    } else {
        memset(devpath, 0, sizeof(devpath));
    }
    return &devpath[0];
}
#endif

static int usr_memcmp(const void *b1, const void *b2, size_t len) {
    memcmp_nr++;
    return memcmp(b1, b2, len);
}

static void *usr_memset(void *b, int c, size_t len) {
    memset_nr++;
    return memset(b, c, len);
}

static void *usr_memcpy(void *dest, const void *src, size_t len) {
    memcpy_nr++;
    return memcpy(dest, src, len);
}
