//
// Created by uservirt on 31/7/18.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common_utils.h"

void strip_chars_generic(char *s, char *strip) {
    char *p = s;
    size_t n;
    while (*s) {
        n = strcspn(s, strip);
        strncpy(p, s, n);
        p += n;
        s += n + strspn(s + n, strip);
    }
    *p = 0;
}

void strip_newline_chars(char *s) {
    strip_chars_generic(s, NEW_LINE_CHARS);
}

char *strip_chars_copy(char const *s, char *strip) {
    char *buf = malloc(1 + strlen(s));
    if (buf) {
        char *p = buf;
        char const *q;
        size_t n;
        for (q = s; *q; q += n + strspn(q + n, strip)) {
            n = strcspn(q, strip);
            strncpy(p, q, n);
            p += n;
        }
        *p++ = '\0';
        buf = realloc(buf, p - buf);
    }
    return buf;
}

char *strip_newline_chars_copy(char const *s) {
    return strip_chars_copy(s, NEW_LINE_CHARS);
}

char *str_concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    if (!result) return result; // return the unsuccessful NULL
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *str_flatten(const char *array[], int start, int end) {
    int count = end - start;
    size_t lengths[count];
    size_t size = 0;
    size_t pos = 0;

    for (int i = start, j = 0; i < end; ++i, ++j) {
        lengths[j] = strlen(array[i]);
        size += lengths[j];
    }

    char *str = malloc(size + count);
    str[size + count - 1] = '\0';

    for (int i = start, j = 0; i < (end - 1); ++i, ++j) {
        memcpy(str + pos + j
                // current index
                , array[i]
                // current index length
                , lengths[j]);
        // add space
        str[pos + lengths[j] + j] = ' ';
        // bump `pos`
        pos += lengths[j];
    }

    memcpy(str + pos + count - 1, array[end - 1], lengths[count - 1]);

    return str;
}

bool str_startswith(const char *tofindin, const char *tofind) {
    if (strncmp(tofindin, tofind, strlen(tofind)) == 0) return 1;
    return 0;
}

int str_occurrences(const char *needle, const char *haystack) {
    if (NULL == needle || NULL == haystack) return -1;

    char *pos = (char *) haystack;
    size_t i = 0;
    size_t l = strlen(needle);

    while ((pos = strstr(pos, needle))) {
        pos += l;
        i++;
    }

    return (int) i;
}

char *str_replace(const char *str, const char *sub, const char *replace) {
    char *pos = (char *) str;
    int count = str_occurrences(sub, str);

    if (0 >= count) return strdup(str);

    int size = (strlen(str) - (strlen(sub) * count) + strlen(replace) * count) + 1;

    char *result = (char *) malloc(size);
    if (NULL == result) return NULL;
    memset(result, '\0', size);
    char *current;
    while ((current = strstr(pos, sub))) {
        int len = current - pos;
        strncat(result, pos, len);
        strncat(result, replace, strlen(replace));
        pos = current + strlen(sub);
    }

    if (pos != (str + strlen(str))) {
        strncat(result, pos, (str - pos));
    }

    return result;
}

int asprintf(char **str, const char *fmt, ...) {
    int size = 0;
    va_list args;

    // init variadic argument
    va_start(args, fmt);

    // format and get size
    size = vasprintf(str, fmt, args);

    // toss args
    va_end(args);

    return size;
}

int vasprintf(char **str, const char *fmt, va_list args) {
    *str = NULL;
    int size = 0;
    va_list tmpa;

    // copy
    va_copy(tmpa, args);

    // apply variadic arguments to
    // sprintf with format to get size
    size = vsnprintf(NULL, (size_t) size, fmt, tmpa);

    // toss args
    va_end(tmpa);

    // return -1 to be compliant if
    // size is less than 0
    if (size < 0) { return -1; }

    // alloc with size plus 1 for `\0'
    *str = (char *) malloc((size_t) (size) + 1);

    // return -1 to be compliant
    // if pointer is `NULL'
    if (NULL == *str) { return -1; }

    // zero fill memory
    memset(*str, 0, (size_t) (size) + 1);

    // format string with original
    // variadic arguments and set new size
    size = vsprintf(*str, fmt, args);
    return size;
}

int file_exist(const char *filename) {
    struct stat b;
    return (stat(filename, &b) == 0);
}

int file_copy(const char *src, const char *dest) {
    char c[64];

    FILE *stream_R = fopen(src, "r");
    if (stream_R == NULL) return -1;

    FILE *stream_W = fopen(dest, "w");
    if (stream_W == NULL) {
        fclose(stream_R);
        return -2;
    }

    while (!feof(stream_R)) {
        size_t bytes = fread(c, 1, sizeof(c), stream_R);
        if (bytes) {
            fwrite(c, 1, bytes, stream_W);
        }
    }

    fclose(stream_R);
    fclose(stream_W);

    return 0;
}