#include "toyson.h"

typedef enum parser_state_ {
    STATE_NONE,
    STATE_OBJECT,
    STATE_ARRAY,
    STATE_STRING
} parser_state;


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
        item->type = TOYSON_TYPE_OBJECT;
        toyson_append_item(entry, item);
        break;
    case '}':
        state = STATE_NONE;
        item->type = TOYSON_TYPE_CLOSE;
        toyson_append_item(entry, item);
        break;
    case '"': {
        state = STATE_STRING;
        toyson_t *last = toyson_last_item(entry);
        if (last->type != TOYSON_TYPE_KEY) {
            item->type = TOYSON_TYPE_KEY;
            ptr = toyson_parse_string(ptr, &item->value);
            toyson_append_item(entry, item);

            ptr = toyson_wind_until_colon(ptr);
        } else {
            item->type = TOYSON_TYPE_STRING;

            ptr = toyson_parse_string(ptr, &item->value);
            toyson_append_item(entry, item);
        }
        break;
    }
    case ',':
        break;
    default:
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
    /* ref->key = NULL; */
    ref->value = NULL;
    ref->prev = NULL;
    ref->next = NULL;
}


void toyson_del(toyson_t *ref)
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

void toyson_print(toyson_t *entry)
{
    toyson_t *ptr = entry->next;
    do {
        char *type = NULL;
        switch (ptr->type) {
        case TOYSON_TYPE_OBJECT:
            type = "Object";
            break;
        case TOYSON_TYPE_CLOSE:
            type = "Close";
            break;
        case TOYSON_TYPE_KEY:
            type = "key";
            break;
        case TOYSON_TYPE_STRING:
            type = "String";
            break;
        default:
            type = "Unknown type";
            break;
        }
        printf("type: %s\n", type);
        switch (ptr->type) {
        case TOYSON_TYPE_KEY:
        case TOYSON_TYPE_STRING:
            printf("value: %s\n", ptr->value);
            break;
        default:
            break;
        }
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

