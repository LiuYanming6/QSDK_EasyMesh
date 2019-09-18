//
// Created by uservirt on 31/7/18.
//

#ifndef COMMON_UTILS_C_LANGUAGE_H
#define COMMON_UTILS_C_LANGUAGE_H

#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NEW_LINE_CHARS "\r\n"

void strip_chars_generic(char *s, char *strip);

void strip_newline_chars(char *s);

char *strip_chars_copy(char const *s, char *strip);

char *strip_newline_chars_copy(char const *s);

char *str_concat(const char *s1, const char *s2);

char *str_flatten(const char *array[], int start, int end);

bool str_startswith(const char *tofindin, const char *tofind);

int str_occurrences(const char *needle, const char *haystack);

char *str_replace(const char *str, const char *sub, const char *replace);

/**
 * Sets `char **' pointer to be a buffer large enough to hold the formatted string
 * accepting a `va_list' args of variadic arguments.
 */
int vasprintf(char **, const char *, va_list);

/**
 * Sets `char **' pointer to be a buffer large enough to hold the formatted
 * string accepting `n' arguments of variadic arguments.
 */
int asprintf(char **, const char *, ...);

int file_exist(const char *filename);

int file_copy(const char *src, const char *dest);


#ifdef __cplusplus
}
#endif

#endif //COMMON_UTILS_C_LANGUAGE_H
