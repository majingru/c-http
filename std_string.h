#ifndef _STD_STRING_H_
#define _STD_STRING_H_
#include "std_common.h"
#include <string.h>

typedef struct STRING
{
    int used;
    int size;
    char *buf;
} String;

String std_string_malloc(int initial_size);

void std_string_free(String str);

bool std_string_equals(const String src, const String dst);

void std_string_appends(String *dst, const char * str, int size);

void std_string_append(String *dst, String * src);

void std_string_appendf(String *dst, char *format, ...);

int std_string_index(String *dst, const char * sub);

char * std_string_sub(String *dst, int start, int end);

char * std_string_to_s(String str);

#endif

