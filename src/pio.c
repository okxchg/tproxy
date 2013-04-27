#include <unistd.h>
#include <errno.h>

#include "pio.h"

int tp_open(const char *filename, int flags, mode_t mode)
{
    return open(filename, flags, mode);
}

int tp_write(int fd, const void *buf, size_t len) 
{
    ssize_t ret;
    const char *p;
    const char *end;

    p = buf;
    end = buf + len;
    while (p < end) {
        ret = write(fd, p, (size_t)(end-p));
        if (ret > 0) {
            p += ret;
        }
        else if (errno != EINTR) {
            return -1;
        }
                
    }
    return 0;
}

/*
ssize_t tp_read(int fd, void *buf, size_t len)
{
    ssize_t ret, rlen = 0;
    char *ptr;

    ptr = buf;
    while (wlen != len) {
        ret = read(fd, ptr, len-rlen);

        if (ret == -1)
            if (errno == EINTR)
                continue;
            else 
                return -1;
        rlen += ret;
        ptr += ret;
    }
}
*/
