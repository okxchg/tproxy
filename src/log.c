#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <syslog.h>

#include "log.h"
#include "pio.h"
#include "../config.h"

#define MAX_TIME_LEN 40
#define MAX_MSG_LEN 512
#define MAX_ERRNO_LEN 512

static int log_fd = -1;
static int log_level;
static enum tp_log_mode log_mode;

static int log_syslog[] = {
    LOG_DEBUG, LOG_INFO, LOG_ERR, LOG_EMERG,
};

void tp_log_init(enum tp_log_mode mode, enum tp_log_level level, int fd)
{
    assert(mode == TPLM_SYSLOG || mode == TPLM_FILE);
    assert(mode == TPLM_FILE ? fd >= 0 : 1);

    switch (mode) {
        case TPLM_FILE:
            log_fd = fd;
            break;
        case TPLM_SYSLOG:
            openlog(TPL_IDENT, LOG_PID|LOG_CONS, LOG_DAEMON);
            break;
    }
    log_level = level;
    log_mode = mode;
}

static void tp_vlog_write(int errnoflag, enum tp_log_level level, 
        const char *fmt, va_list alist)
{
    int ret;
    int len;
    int errno_saved;
    time_t t;
    struct tm *tm;
    char msg[MAX_TIME_LEN+MAX_MSG_LEN+MAX_ERRNO_LEN+2];

    assert(log_fd >= 0);
    assert(fmt);

    if (level < log_level)
        return;

    errno_saved = errno;

    switch (log_mode) {
        case TPLM_FILE:

            t = time(NULL);
            tm = localtime(&t);
            if (tm != NULL) 
                len = strftime(msg, MAX_TIME_LEN, "%a, %d %b %Y %T %z: ", tm);
            else
                len = 0;

            ret = vsnprintf(msg+len, MAX_MSG_LEN, fmt, alist);
            if (ret >= MAX_MSG_LEN) 
                len += MAX_MSG_LEN-1;
            else
                len += ret;

            if (errnoflag) {
                ret = snprintf(msg+len, MAX_ERRNO_LEN, " (errno: %s)", 
                        strerror(errno_saved));
                if (ret >= MAX_ERRNO_LEN)
                    len += MAX_ERRNO_LEN-1;
                else
                    len += ret;
            }
            strcpy(msg+len++, "\n");
            tp_write(log_fd, msg, len+1);
            break;

        case TPLM_SYSLOG:
#ifdef HAVE_VSYSLOG
        vsyslog(log_syslog[level], fmt, alist);
#else
        vsnprintf(msg, MAX_MSG_LEN, fmt, vl);
        syslog(log_syslog[level], "%s", msg);
#endif
            break;
    }
}

void tp_log_write(enum tp_log_level level, const char *fmt, ...)
{
   va_list vl;

   va_start(vl, fmt);
   tp_vlog_write(0, level, fmt, vl);
   va_end(vl);
}

void tp_sys_error(enum tp_log_level level, const char *fmt, ...)
{
   va_list vl;

   va_start(vl, fmt);
   tp_vlog_write(1, level, fmt, vl);
   va_end(vl);
}

void tp_log_close(void)
{
    assert(log_fd >= 0);

    switch (log_mode) {
        case TPLM_SYSLOG: 
            closelog();
            break;
        case TPLM_FILE:
            close(log_fd);
            break;
    }
}
