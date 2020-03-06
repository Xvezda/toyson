#ifndef TOYSON_H_
#define TOYSON_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>


typedef struct toyson_s {
  char *key;
  char *value;
  struct toyson_s *next;
} toyson_t;


toyson_t *toyson_new(void);
void toyson_init(toyson_t **ref);
void toyson_del(toyson_t **ref);
toyson_t *toyson_parser(char *text);

char *toyson_parse_string(char *text, char **ref);
char *toyson_skip_space(char *ptr);
char *toyson_wind_until_quote(char *ptr);
char *toyson_wind_until_colon(char *ptr);


#endif  // TOYSON_H_
