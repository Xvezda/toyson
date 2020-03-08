/**
 * Copyright (c) 2020 Xvezda <xvezda@naver.com>
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "toyson.h"


static void read_from_file(char **ref, char *fname);


static int is_file;

int main(int argc, char **argv)
{
    int count = 0;
    char *json = NULL;

    if (argc < 2) {
        count = 1;
        json = \
            "{\n"
            "  \"foo\" : \"bar:bar\","
            "  \"baz\": {"
                "\"hello\": \"world\","
                " \"qwer\": 12345"
                "},\n"
            "\t\"asdf\": [123, 456, \"789\"],\n"
            "\"zxcv\": [[{}, {"
                    "\"abc\": true, \"def\": false"
                "}, \"uiop\", null, \"\" ], \"hjkl\", true ]\n"
            "}";
    } else {
        if (!strncmp(argv[1], "-h", 2) || !strncmp(argv[1], "--help", 6)) {
            fprintf(stderr, "usage: %s [JSON FILES]\n", argv[0]);

            return EXIT_FAILURE;
        }

        is_file = 1;
        count = argc - 1;
    }

    for (int i = 0; i < count; ++i) {
        if (is_file) read_from_file(&json, argv[i+1]);

        toyson_t toyson;

        toyson_init(&toyson);
        toyson_parser(&toyson, json);

        printf("json: %s\n", json);
        toyson_print(&toyson);

        toyson_del(&toyson);

        if (is_file) free(json);
    }

    return EXIT_SUCCESS;
}


static void read_from_file(char **ref, char *fname)
{
    FILE *fp;
    if ((fp = fopen(fname, "r"))) {
        fseek(fp, 0, SEEK_END);
        size_t fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        *ref = malloc(fsize + 1);

        fread(*ref, 1, fsize, fp);
        fclose(fp);

        (*ref)[fsize] = '\0';
    } else {
        is_file = 0;
        *ref = "{}";
    }
}

