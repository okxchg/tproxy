#ifndef URI_H
#define URI_H

#define HTTP_URI_SCHEME "http"
#define HTTP_DEFAULT_PORT 80

struct uri_authority {
    char *opt;
    char *hostname;
    int port;
};

struct uri {
    char *scheme;
    struct uri_authority authority;
    char *path;
    char *query;
    char *fragment;
};

int uri_parse(struct uri *uri, const char *uri_s);
void uri_destroy(struct uri *uri);

#endif
