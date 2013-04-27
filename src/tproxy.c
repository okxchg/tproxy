#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "log.h"

int main(int argc, const char *argv[])
{
    int sock_fd;
    int conn_fd;
    struct sockaddr_in saddr;
    struct sockaddr_in caddr;
    int clen;
    
    tp_log_init(TPLM_FILE, TPL_DEBUG, STDERR_FILENO);
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        tp_sys_error(TPL_EMERG, "Cannot create socket");
        exit(1);
    }

    saddr.sin_family = htonl(PF_INET);
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&saddr, 
                sizeof(struct sockaddr)) == -1) {
        tp_sys_error(TPL_EMERG, "Cannot bind socket");
        close(sock_fd);
        exit(1);
    }

    if (listen(sock_fd, SOMAXCONN) == -1) {
        tp_sys_error(TPL_EMERG, "Cannot listen on socket");
        close(sock_fd);
        exit(1);
    }

    for(;;) {
        conn_fd = accept(sock_fd, (struct sockaddr *)&caddr, &clen);

        if (conn_fd > 0) {
            tp_log_write(TPL_DEBUG, "Got connection");
        }
        else {
            tp_sys_error(TPL_EMERG, "Accept error");
        }

    }
    return 0;
}
