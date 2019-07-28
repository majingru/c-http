#ifndef LOG_H
#define LOG_H
#include "std_common.h"
#include <stdio.h>
#include <stdarg.h>

#define LOG_VERSION "0.1.1"

typedef void (*log_LockFn)(void *udata, int lock);

typedef enum { LOG_DEBUG, LOG_INFO, LOG_ERROR, LOG_OFF} log_level;

#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

void log_set_udata(void *udata);
void log_set_lock(log_LockFn fn);
void log_set_file(const char * file);
void log_set_level(const char * level);
void log_close_file();
void log_log(log_level level, const char *file, int line, const char *fmt, ...);

#endif
