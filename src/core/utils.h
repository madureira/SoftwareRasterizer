#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#include <ctype.h>
#include <string.h>

static inline char* str_trim_left(const char* text)
{
    while (*text != '\0' && isspace((unsigned char)*text))
    {
        text++;
    }

    return (char*)text;
}

static inline void str_trim_right(char* text)
{
    usize length = strlen(text);

    while (length > 0 && isspace((unsigned char)text[length - 1]))
    {
        text[length - 1] = '\0';
        length--;
    }
}

static inline char* str_trim(char* text)
{
    text = str_trim_left(text);
    str_trim_right(text);

    return text;
}

static inline bool str_equals(const char* a, const char* b)
{
    return strcmp(a, b) == 0;
}

#endif // CORE_UTILS_H
