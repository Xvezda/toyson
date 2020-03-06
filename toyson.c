#include "toyson.h"

typedef enum parser_state_ {
    STATE_NONE,
    STATE_OBJECT,
    STATE_ARRAY,
    STATE_STRING
} parser_state;


toyson_t *toyson_parser(char *text)
{
    parser_state state = STATE_NONE;
    toyson_t *ret = toyson_new();

    for (char *ptr = text; *ptr; ++ptr) {
        switch (*ptr) {
        case '{': {
            state = STATE_OBJECT;
            ptr = toyson_skip_space(ptr);

            /* Parse key */
            ptr = toyson_parse_string(ptr+1, &ret->key);
            ptr = toyson_wind_until_colon(ptr+1);

            /* Parse value */
            ptr = toyson_parse_string(ptr+1, &ret->value);

            break;
        }
        default:
            break;
        }
    }
    return ret;
}


toyson_t *toyson_new()
{
    toyson_t *ret = malloc(sizeof (toyson_t));
    toyson_init(&ret);

    return ret;
}

void toyson_init(toyson_t **ref)
{
    (*ref)->key = NULL;
    (*ref)->value = NULL;
    (*ref)->next = NULL;
}


void toyson_del(toyson_t **ref)
{
}


char *toyson_parse_string(char *text, char **ref)
{
    if (!ref) return NULL;
    char *cur = text;
    cur = toyson_wind_until_quote(cur);

    char *start = cur + 1;
    char *end = toyson_wind_until_quote(cur+1);

    size_t len = (size_t) (end-start);
    assert(len > 0);

    *ref = malloc(len + 1 /* NULL */);

    strncpy(*ref, start, len);
    (*ref)[len] = '\0';

    return end;
}


char *toyson_skip_space(char *ptr)
{
    char *cur = ptr;
    while (*cur && isspace(*cur)) ++cur;

    return cur;
}

char *toyson_wind_until_quote(char *ptr)
{
    char *cur = ptr;
    while (*cur && *cur != '"') {
        if (*cur == '\\') ++cur;
        ++cur;
    }
    return cur;
}


char *toyson_wind_until_colon(char *ptr)
{
    char *cur = ptr;
    while (*cur && *cur != ':') ++cur;

    return cur;
}

