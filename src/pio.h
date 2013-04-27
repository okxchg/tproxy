#ifndef PIO_H
#define PIO_H 

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

int tp_open(const char *filename, int flags, mode_t mode);
ssize_t tp_write(int fd, const void *buf, size_t len) ;
ssize_t tp_read(int fd, void *buf, size_t len);

#endif
