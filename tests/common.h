#ifndef COMMON_H
#define COMMON_H 

#include "../src/uri.h"

#define assert_str(a, b) \
    if (!b || !a) { fail_if(a != b); }\
    else { fail_if(strcmp(a, b)); }

#define assert_str_msg(a, b, diag) \
    if (!b || !a) { fail_if(a != b, diag); }\
    else { fail_if(strcmp(a, b), diag); }

void assert_uri(struct uri *uri, const char *uri_s, char *scheme, char *opt, 
        char *hostname, int port, char *path, char *query, char *fragment);

#endif
