#ifndef _STD_COMMON_H_
#define _STD_COMMON_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#define STD_OS_WIN
#else
#define STD_OS_UX
#endif

typedef enum { false = 0,
               true = 1 } bool;

/**
 * @brief memory operator.
 * @param size
 */
void *std_malloc(size_t size);
void std_free(void *ptr);
void *std_realloc(void *old_ptr, size_t size);

/**
 * @brief system utils.
 * @param hook
 */
typedef void (*shutdown_hook)(void);

void std_shutdown_hook(shutdown_hook hook);


/**
 * @brief time utils.
 * @return 
 */
double std_current_second();

double std_current_millisecond();

/**
 * @brief file util.
 * @param file
 * @param flags
 * @param mode
 * @return 
 */
FILE *std_file_append(const char *file);

FILE *std_file_truncate(const char *file);

FILE *std_out();

FILE *std_err();

FILE *std_in();

void std_close(FILE *fd);

#endif

