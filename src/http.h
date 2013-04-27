#ifndef HTTP_H
#define HTTP_H 

#include "uri.h"

struct http_version 
{
    short major;
    short minor;
};

struct http_request_line
{
    char *method;
    struct uri uri;
    struct http_version version;
};

struct http_status_line
{
    struct http_version version;
    short code;
    char *reason;
};

typedef struct http_hdr_table http_hdr_table;

struct http_request
{
    http_hdr_table *headers;
    struct http_request_line rline;
};

struct http_response
{
    http_hdr_table *headers;
    struct http_status_line sline;
};

int http_request_line_parse(struct http_request_line *rline, const char *s);
void http_request_line_destroy(struct http_request_line *rline);

int http_status_line_parse(struct http_status_line *sline, const char *s);
void http_status_line_destroy(struct http_status_line *sline);

http_hdr_table *http_headers_init(void);
void http_headers_destroy(http_hdr_table *t);

int http_header_parse(http_hdr_table *t, const char *s);
char *http_header_get(http_hdr_table *t, const char *header);

#endif
