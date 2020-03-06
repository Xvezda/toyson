#include <stdio.h>
#include "toyson.h"


int main(int argc, char **argv)
{
    char *json = "{\n  \"foo\": \"bar\"\n}";

    toyson_t *result = toyson_parser(json);
    printf("json: %s\n", json);
    printf("key: %s, value: %s\n", result->key, result->value);

    return 0;
}
