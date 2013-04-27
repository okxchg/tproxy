#ifndef LOG_H
#define LOG_H 

#include <syslog.h>

#include "../config.h"

#define TPL_IDENT PACKAGE_NAME

enum tp_log_mode {
    TPLM_SYSLOG,
    TPLM_FILE,
};

enum tp_log_level {
    TPL_DEBUG,
    TPL_INFO,
    TPL_ERR,
    TPL_EMERG,
};

void tp_log_init(enum tp_log_mode mode, enum tp_log_level level, int fd);
void tp_log_write(enum tp_log_level level, const char *fmt, ...);
void tp_sys_error(enum tp_log_level level, const char *fmt, ...);
void tp_log_close(void);

#endif
