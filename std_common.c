#include "std_common.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

void *std_malloc(size_t size)
{
    if (size <= 0)
    {
        return NULL;
    }
    void *buf = calloc(size, 1);
    memset(buf, 0, size);
    return buf;
}

void std_free(void *ptr)
{
    if (ptr)
    {
        free(ptr);
        ptr = NULL;
    }
}

void *std_realloc(void *old_ptr, size_t size)
{
    if (size <= 0)
    {
        return old_ptr;
    }
    if (old_ptr)
    {
        return realloc(old_ptr, size);
    }
    else
    {
        return std_malloc(size);
    }
}

void std_shutdown_hook(shutdown_hook hook)
{
    if (hook)
    {
        atexit(hook);
    }
}

double std_current_second()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

double std_current_millisecond()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec * 1000  + (double)tv.tv_usec/1000;
}

FILE *std_file_append(const char *file)
{
    if (file)
    {
        return fopen(file, "a+");
    }
    return NULL;
}

FILE *std_file_truncate(const char *file)
{
    if (file)
    {
        return fopen(file, "w+");
    }
    return NULL;
}

void std_close(FILE *fd)
{
    if (fd > 0)
    {
        fclose(fd);
    }
}

FILE *std_out()
{
    return stdout;
}

FILE *std_err()
{
    return stderr;
}

FILE *std_in()
{
    return stdin;
}

