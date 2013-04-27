#include <check.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "../src/log.h"

START_TEST(test_log)
{
    int fd;
    char c;
    int ret = 0;
    int len = 0;
    char msg[1024];
    char *p;

    fd = shm_open("/test_log", O_RDWR|O_CREAT, 0600);
    if (fd == -1)
        fail("failed to initialize");

    tp_log_init(TPLM_FILE, TPL_EMERG, fd);
    tp_log_write(TPL_DEBUG, "debug message");
    tp_log_write(TPL_INFO, "info message");
    tp_log_write(TPL_EMERG, "emerg message");
    lseek(fd, 0, SEEK_SET);

    while ((ret = read(fd, msg+len, 512)) != 0) {
        if (ret == -1) fail("error while reading");
        len += ret;
    }
    msg[len-1] = 0;
    //printf("%s\n", msg);

    for (p = msg;; p++) {
        if (*p == ':' && *(p+1) == ' ') {
            fail_if(strcmp(p, ": emerg message\n") != 0, p);
            break;
        }
        if (!*p) fail("bad log string");
    }

    shm_unlink("test_log");
} END_TEST

Suite *log_suite(void)
{
    Suite *s = suite_create("log");

    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_log);
    suite_add_tcase(s, tc_core);

    return s;
}
