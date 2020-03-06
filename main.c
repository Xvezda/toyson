/**
 * Copyright (c) 2020 Xvezda <xvezda@naver.com>
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#include <stdio.h>
#include "toyson.h"


int main(int argc, char **argv)
{
    char *json = \
        "{\n"
        "  \"foo\": \"bar\","
        "  \"baz\": {"
            "\"hello\": \"world\","
            " \"qwer\": 12345"
            "},\n"
        "\t\"asdf\": [123, 456, \"789\"],\n"
        "\"zxcv\": [[{}, {"
                "\"abc\": true, \"def\": false"
            "}, \"uiop\"], \"hjkl\", true ]\n"
        "}";

    toyson_t toyson;
    toyson_init(&toyson);
    toyson_parser(&toyson, json);

    printf("json: %s\n", json);
    toyson_print(&toyson);

    toyson_del(&toyson);

    return 0;
}
