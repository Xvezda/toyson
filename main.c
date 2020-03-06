#include <stdio.h>
#include "toyson.h"


int main(int argc, char **argv)
{
    char *json = "{\n  \"foo\": \"bar\", \"baz\": {\"hello\": \"world\"}\n}";

    toyson_t toyson;
    toyson_init(&toyson);
    toyson_parser(&toyson, json);

    printf("json: %s\n", json);
    toyson_print(&toyson);

    return 0;
}
