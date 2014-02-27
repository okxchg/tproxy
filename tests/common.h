#ifndef COMMON_H
#define COMMON_H 

#include "../src/uri.h"

#define s(s) s == NULL ? "(null)" : s

void assert_uri(struct uri *uri, const char *uri_s, char *scheme, char *opt, 
        char *hostname, int port, char *path, char *query, char *fragment);

void assert_str(const char *a, const char *b);
void assert_str_msg(const char *a, const char *b, const char *diag);

#endif
