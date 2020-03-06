/**
 * Copyright (c) 2020 Xvezda <xvezda@naver.com>
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#ifndef TOYSON_H_
#define TOYSON_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>


typedef enum {
  TOYSON_TYPE_ENTRY,
  TOYSON_TYPE_KEY,
  TOYSON_TYPE_OBJECT_OPEN,   /* Open object */
  TOYSON_TYPE_OBJECT_CLOSE,  /* Closing object */
  TOYSON_TYPE_ARRAY_OPEN,
  TOYSON_TYPE_ARRAY_CLOSE,
  TOYSON_TYPE_NULL,
  TOYSON_TYPE_NUMBER,
  TOYSON_TYPE_STRING,
  TOYSON_TYPE_BOOLEAN,
} toyson_data_type_t;

typedef struct toyson_s {
  toyson_data_type_t type;
  char *value;
  struct toyson_s *prev;
  struct toyson_s *next;
} toyson_t;

typedef int (*toyson_until_handler_t)(char *);

toyson_t *toyson_new(void);
void toyson_init(toyson_t *ref);
void toyson_del(toyson_t *ref);
void toyson_parser(toyson_t *entry, char *text);
void toyson_append_item(toyson_t *entry, toyson_t *item);
toyson_t *toyson_last_item(toyson_t *entry);
void toyson_print(toyson_t *entry);

char *toyson_parse_null(char *text, char **ref);
char *toyson_parse_boolean(char *text, char **ref);
char *toyson_parse_number(char *text, char **ref);
char *toyson_parse_string(char *text, char **ref);

char *toyson_skip_space(char *ptr);
char *toyson_wind_until(char *ptr, toyson_until_handler_t handler);
char *toyson_wind_until_space(char *ptr);
char *toyson_wind_until_quote(char *ptr);
char *toyson_wind_until_comma(char *ptr);
char *toyson_wind_until_colon(char *ptr);


#endif  // TOYSON_H_
