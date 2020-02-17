/*
 *                          Copyright (C) 2020 by Rafael Santiago
 *
 * Use of this source code is governed by GPL-v2 license that can
 * be found in the COPYING file.
 *
 */
#include <cutest.h>
#include <lethe_strglob.h>
#include <lethe_mkpath.h>
#include <lethe_random.h>
#include <lethe_error.h>
#if defined(LETHE_TOOL)
# include <lethe_option.h>
#endif
#include <lethe_drop.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int stat_call_nr = 0;

static int stat_wrapper(const char *pathname, struct stat *buf) {
    stat_call_nr++;
    return stat(pathname, buf);
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

CUTE_DECLARE_TEST_CASE(lethe_tests_entry);

CUTE_DECLARE_TEST_CASE(lethe_strglob_tests);
CUTE_DECLARE_TEST_CASE(lethe_mkpath_tests);
CUTE_DECLARE_TEST_CASE(lethe_default_randomizer_tests);
CUTE_DECLARE_TEST_CASE(lethe_error_stuff_tests);
#if defined(LETHE_TOOL)
CUTE_DECLARE_TEST_CASE(lethe_option_stuff_tests);
#endif
CUTE_DECLARE_TEST_CASE(lethe_drop_tests);

CUTE_MAIN(lethe_tests_entry)

CUTE_TEST_CASE(lethe_tests_entry)
    CUTE_RUN_TEST(lethe_strglob_tests);
    CUTE_RUN_TEST(lethe_mkpath_tests);
    CUTE_RUN_TEST(lethe_default_randomizer_tests);
    CUTE_RUN_TEST(lethe_error_stuff_tests);
#if defined(LETHE_TOOL)
    CUTE_RUN_TEST(lethe_option_stuff_tests);
#endif
    CUTE_RUN_TEST(lethe_drop_tests);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(lethe_drop_tests)
    char *buf, *temp;
    struct stat st;

    CUTE_ASSERT(lethe_set_stat(NULL) != 0);
    CUTE_ASSERT(lethe_set_stat(stat_wrapper) == 0);
    CUTE_ASSERT(lethe_set_rename_nr(0) != 0);
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // WARN(Rafael): At this point do not remove the chdir stuff otherwise you can lose important files.!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    CUTE_ASSERT(mkdir("lethe-lab", 0666) == 0);
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
    // TODO(Rafael): Test drop by passing kLetheFileRemove and kLetheCustomRandomizer.
    CUTE_ASSERT(remove("data.txt") == 0);
    CUTE_ASSERT(chdir("..") == 0);
    CUTE_ASSERT(rmdir("lethe-lab") == 0);
    CUTE_ASSERT(stat_call_nr > 0);
CUTE_TEST_CASE_END

#if defined(LETHE_TOOL)
CUTE_TEST_CASE(lethe_option_stuff_tests)
    char *argv[] = {
        "./lethe",
        "--foo=bar",
        "--bar=foo",
        "--foobar"
    };
    int argc = 4;
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

    lethe_option_set_argc_argv(argc, argv);

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
        { "hell yeah!",                 "*yeah!"                                                   , 1 }
    };
    size_t tests_nr = sizeof(tests) / sizeof(tests[0]), t;

    for (t = 0; t < tests_nr; t++) {
        CUTE_ASSERT(lethe_strglob(tests[t].str, tests[t].pattern) == tests[t].result);
    }
CUTE_TEST_CASE_END
