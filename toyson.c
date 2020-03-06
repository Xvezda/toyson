/**
 * Copyright (c) 2020 Xvezda <xvezda@naver.com>
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#include "toyson.h"


typedef enum parser_state_ {
    STATE_NONE,
    STATE_OBJECT,
    STATE_ARRAY,
    STATE_STRING
} parser_state;


static int toyson_not_space(char *ptr);
static int toyson_not_colon(char *ptr);
static int toyson_not_comma(char *ptr);


static parser_state state = STATE_NONE;

void toyson_parser(toyson_t* entry, char *text)
{
    if (!text) return;

    char *ptr = toyson_skip_space(text);
    toyson_t *item = toyson_new();

    assert(item->value == NULL);

    if (!*ptr) return;

    switch (*ptr) {
    case '{':
        state = STATE_OBJECT;
        item->type = TOYSON_TYPE_OBJECT_OPEN;
        toyson_append_item(entry, item);
        break;
    case '}':
        state = STATE_NONE;
        item->type = TOYSON_TYPE_OBJECT_CLOSE;
        toyson_append_item(entry, item);
        break;
    case '[':
        item->type = TOYSON_TYPE_ARRAY_OPEN;
        toyson_append_item(entry, item);
        break;
    case ']':
        item->type = TOYSON_TYPE_ARRAY_CLOSE;
        toyson_append_item(entry, item);
        break;
    case '"': {
        state = STATE_STRING;
        toyson_t *last = toyson_last_item(entry);

        char *tmp = toyson_wind_until_quote(ptr+1);
        char *comma = toyson_wind_until_comma(tmp);
        char *colon = toyson_wind_until_colon(tmp);

        /*
         * If colon does not exists, or comma is closer than colon
         */
        if (!*colon || colon > comma) {
            item->type = TOYSON_TYPE_STRING;

            ptr = toyson_parse_string(ptr, &item->value);
            toyson_append_item(entry, item);
        } else {
            item->type = TOYSON_TYPE_KEY;
            ptr = toyson_parse_string(ptr, &item->value);
            toyson_append_item(entry, item);

            ptr = toyson_wind_until_colon(ptr);
        }
        break;
    }
    case ',':
        break;
    case 't':  /* true */
    case 'f':  /* false */
        item->type = TOYSON_TYPE_BOOLEAN;
        ptr = toyson_parse_boolean(ptr, &item->value);
        toyson_append_item(entry, item);
        break;
    case 'n':  /* null */
        item->type = TOYSON_TYPE_NULL;
        ptr = toyson_parse_null(ptr, &item->value);
        toyson_append_item(entry, item);
        break;
    default:
        if (isdigit(*ptr)) {
            item->type = TOYSON_TYPE_NUMBER;
            ptr = toyson_parse_number(ptr, &item->value);
            toyson_append_item(entry, item);
        }
        break;
    }
    toyson_parser(entry, ptr+1);
}


void toyson_append_item(toyson_t *entry, toyson_t *item)
{
    toyson_t *last = toyson_last_item(entry);
    last->next = item;
    item->prev = last;
}


toyson_t *toyson_last_item(toyson_t *entry)
{
    toyson_t *ptr = entry->next;
    if (!ptr) return entry;
    while (ptr->next) ptr = ptr->next;
    return ptr;
}


toyson_t *toyson_new()
{
    toyson_t *ret = malloc(sizeof (toyson_t));
    toyson_init(ret);

    return ret;
}


void toyson_init(toyson_t *ref)
{
    ref->value = NULL;
    ref->prev = NULL;
    ref->next = NULL;
}


void toyson_del(toyson_t *ref)
{
    toyson_t *last = toyson_last_item(ref);
    toyson_t *tmp = NULL;
    toyson_t *ptr = last;

    while (ptr->prev && ptr->prev != ref) {
        tmp = ptr;
        ptr = ptr->prev;

        free(tmp->value);
        tmp->value = NULL;
        tmp->next = NULL;

        free(tmp);
    }
    ptr->next = NULL;
}


char *toyson_parse_null(char *text, char **ref)
{
    char *cur = text;

    char *start = cur;
    char *end = start;

    char *n = "null";

    if (!strncmp(cur, n, strlen(n))) {
        end += strlen(n);
    } else {
        end = toyson_wind_until_space(start);
    }
    size_t len = (size_t) (end-start);

    assert(len > 0);

    *ref = malloc(len + 1 /* NULL */);

    strncpy(*ref, start, len);
    (*ref)[len] = '\0';

    return end - 1;
}


char *toyson_parse_boolean(char *text, char **ref)
{
    char *cur = text;

    char *start = cur;
    char *end = start;

    char *t = "true";
    char *f = "false";

    if (!strncmp(cur, t, strlen(t))) {
        end += strlen(t);
    } else if (!strncmp(cur, f, strlen(f))) {
        end += strlen(f);
    } else {
        end = toyson_wind_until_space(start);
    }
    size_t len = (size_t) (end-start);

    assert(len > 0);

    *ref = malloc(len + 1 /* NULL */);

    strncpy(*ref, start, len);
    (*ref)[len] = '\0';

    return end - 1;
}


char *toyson_parse_number(char *text, char **ref)
{
    char *start = text;

    char *cur = start;
    while (*cur && isdigit(*cur)) ++cur;
    char *end = cur;

    size_t len = (size_t) (end-start);
    assert(len > 0);

    *ref = malloc(len + 1 /* NULL */);

    strncpy(*ref, start, len);
    (*ref)[len] = '\0';

    return end - 1;
}


char *toyson_parse_string(char *text, char **ref)
{
    if (!ref) return NULL;
    char *cur = text;
    cur = toyson_wind_until_quote(cur);

    char *start = cur + 1;
    char *end = toyson_wind_until_quote(cur+1);

    size_t len = (size_t) (end-start);
    assert(len >= 0);

    if (len > 0) {
        *ref = malloc(len + 1 /* NULL */);

        strncpy(*ref, start, len);
        (*ref)[len] = '\0';
    } else {
        *ref = NULL;
    }
    return end;
}

void toyson_print(toyson_t *entry)
{
    toyson_t *ptr = entry->next;

    if (!ptr) return;
    do {
        char *type = NULL;
        char *value = "";

        switch (ptr->type) {
        case TOYSON_TYPE_OBJECT_OPEN:
            type = "Open Object";
            value = "{";
            break;
        case TOYSON_TYPE_OBJECT_CLOSE:
            type = "Close Object";
            value = "}";
            break;
        case TOYSON_TYPE_ARRAY_OPEN:
            type = "Open Array";
            value = "[";
            break;
        case TOYSON_TYPE_ARRAY_CLOSE:
            type = "Close Array";
            value = "]";
            break;
        case TOYSON_TYPE_KEY:
            type = "Key";
            value = ptr->value;
            break;
        case TOYSON_TYPE_NULL:
            type = "Null";
            value = ptr->value;
            break;
        case TOYSON_TYPE_STRING:
            type = "String";
            value = ptr->value;
            break;
        case TOYSON_TYPE_BOOLEAN:
            type = "Boolean";
            value = ptr->value;
            break;
        case TOYSON_TYPE_NUMBER:
            type = "Number";
            value = ptr->value;
            break;
        default:
            type = "Unknown type";
            break;
        }
        printf("type: %s, value: %s\n", type, value);
        if (ptr && ptr->next) {
            ptr = ptr->next;
            continue;
        }
        break;
    } while (1);
}

char *toyson_skip_space(char *ptr)
{
    if (!ptr) return NULL;

    char *cur = ptr;
    while (*cur && isspace(*cur)) ++cur;

    return cur;
}


char *toyson_wind_until(char *ptr, toyson_until_handler_t handler)
{
    if (!ptr) return NULL;

    char *cur = ptr;
    while (*cur && handler(cur)) ++cur;

    return cur;
}


int toyson_not_space(char *ptr)
{
    return !isspace(*ptr);
}


char *toyson_wind_until_space(char *ptr)
{
    return toyson_wind_until(ptr, toyson_not_space);
}


int toyson_not_comma(char *ptr)
{
    return *ptr != ',';
}

char *toyson_wind_until_comma(char *ptr)
{
    return toyson_wind_until(ptr, toyson_not_comma);
}


int toyson_not_colon(char *ptr)
{
    return *ptr != ':';
}


char *toyson_wind_until_colon(char *ptr)
{
    return toyson_wind_until(ptr, toyson_not_colon);
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



