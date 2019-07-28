#include "std_string.h"
#include <string.h>
#include <stdarg.h>

String std_string_malloc(int initial_size)
{
    String string;
    string.buf = (char*)std_malloc(initial_size * sizeof(char));
    if (string.buf == NULL)
    {
        string.size = 0;
    }
    else
    {
        string.size = initial_size;
    }
    string.used = 0;

    return string;
}

void std_string_free(String str)
{
    std_free(str.buf);
    str.size = 0;
    str.buf = NULL;
    str.used = 0;
}

bool std_string_equals(const String src, const String dst)
{
    return (src.size == dst.size && src.used == dst.used && !memcmp(src.buf, dst.buf, src.size));
}

static bool buffer_grow(String *str, int minimum_size)
{
    int factor = str->size;
    if (factor < minimum_size)
    {
        factor = minimum_size;
    }

    int new_size = factor * 2;
    char *tmp = (char*)std_realloc(str->buf, new_size);
    if (tmp)
    {
        str->buf = tmp;
        str->size = new_size;
        return true;
    }
    else
    {
        return false;
    }
}

void std_string_append(String *dst, String *src)
{
    if (dst->used + src->used > dst->size)
    {
        bool status = buffer_grow(dst, dst->used + src->used + 1);
        if (!status)
        {
            perror("memory malloc failed.\n");
            exit(1);
        }
    }
    int pos = dst->used;
    for (int i = 0; i < src->used; i++)
    {
        dst->buf[i + pos] = src->buf[i];
    }
    dst->used += src->used;
}

void std_string_appends(String *dst, const char *str, int size)
{
    if ((dst->size - dst->used) < size)
    {
        bool status = buffer_grow(dst, size);
        if (!status)
        {
            perror("memory malloc failed.\n");
            exit(1);
        }
    }
    int position = dst->used;
    for (int i = 0; i < size; i++)
    {
        dst->buf[i + position] = str[i];
    }
    dst->used += size;
}

void std_string_appendf(String *dst, char *format, ...)
{
    char tmp[BUFSIZ];
    int n;
    va_list ap;
    va_start(ap, format);
    n = vsnprintf(tmp, BUFSIZ, format, ap);
    va_end(ap);

    std_string_appends(dst, tmp, n);
}

int std_string_index(String *dst, const char * sub)
{
    int sub_len = strlen(sub);
    if(sub_len == 0)
    {
        return 0;
    }
    int dst_len = dst->used;

    for(int i = 0; i < dst_len; i++)
    {
        if(!memcmp(dst->buf + i, sub, sub_len))
        {
            return i;
        }
    }
    return 0;
}

char * std_string_sub(String *dst, int start, int end)
{
    if(dst == NULL || end < start || end > dst->used)
    {
        return NULL;
    }

    int size = end - start;
    char * tmp = (char*)std_malloc(end - start + 1);
    strncpy(tmp, dst->buf + start, size);
    tmp[size] = '\0';
    return tmp;
}

char *std_string_to_s(String str)
{
    char *tmp = (char*)std_malloc(str.used + 1);
    strncpy(tmp, str.buf, str.used);
    tmp[str.used] = '\0';
    return tmp;
}

