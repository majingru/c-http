#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "std_log.h"

static struct {
  void *udata;
  log_LockFn lock;
  FILE *fp;
  log_level level;
} L;


static const char *level_names[] = {
  "DEBUG", "INFO", "ERROR"
};

static void lock(void)   {
  if (L.lock) {
    L.lock(L.udata, 1);
  }
}


static void unlock(void) {
  if (L.lock) {
    L.lock(L.udata, 0);
  }
}


void log_set_udata(void *udata) {
  L.udata = udata;
}


void log_set_lock(log_LockFn fn) {
  L.lock = fn;
}


void log_set_file(const char * log_file) {
  if (log_file)
    {

        FILE *fp;
        if (strcmp(log_file, "std_out") == 0)
        {
            fp = std_out();
        }
        else if (strcmp(log_file, "std_err") == 0)
        {
            fp = std_err();
        }
        else
        {
            fp = std_file_append(log_file);
        }
         L.fp = fp;
    }
    else
    {
        L.fp = std_err();
    }
}
void log_close_file() {
 int ret = -1;
 if( (ret =fclose(L.fp)) != 0)
 {
     printf("Error in closing file \n");
 }
}
void log_set_level(const char * level) {
  log_level l;

  if((level == NULL)|| (strcmp(level, "debug") == 0) || (strcmp(level, "DEBUG") == 0))
  {
    l = LOG_DEBUG;  
  }else if((strcmp(level, "info") == 0) || (strcmp(level, "INFO") == 0)){
    l = LOG_INFO;
  }else if(strcmp(level, "error") == 0 || strcmp(level, "ERROR") == 0){
    l = LOG_ERROR;
  }else{
    l = LOG_OFF;
  }

  L.level = l;
}


void log_log(log_level level, const char *file, int line, const char *fmt, ...) {

  if(L.fp == NULL)
  {
	L.fp = std_err();
  }
  if(level < L.level)
  {
	return;
  }

  /* Acquire lock */
  lock();

  /* Get current time */
  time_t t = time(NULL);
  struct tm *lt = localtime(&t);

  if(L.fp == NULL){
    L.fp = std_err();
  }
	
  /* Log to file */
  if (L.fp) {
    va_list args;
    char buf[64];
    buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';
    fprintf(L.fp, "%s %-5s %s:%d: ", buf, level_names[level], file, line);
    va_start(args, fmt);
    vfprintf(L.fp, fmt, args);
    va_end(args);
    fprintf(L.fp, "\n");
    fflush(L.fp);
  }

  /* Release lock */
  unlock();
}
