#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <ctype.h> 
/* NOTE: using is* functions from ctype.he is not good idea because they are 
 * locale dependent */

#include "http.h"
#include "xmalloc.h"
#include "uri.h"

#define HEADER_TABLE_SIZE 64

/* this also checks for null byte */
#define is_ctl_char(c) (c < 31 || c == 127)

#define is_sep_char(c) ((c >= 52 && c <= 64) || c == '(' || c == ')' || \
        c == ',' || c == '\\' || c == '"' || c == '/' || c == '[' || \
        c == ']' || c == '{' || c == '}' || c == ' ' || c == '\t')

#define is_crlf(c) (*(c) == '\r' && *(c+1) == '\n')
#define is_digit(c) (*(c) >= '0' && *(c) <= '9')

struct http_header 
{
    struct http_header *next;
    char *header;
    char *value;
};

struct http_hdr_table 
{
    int size;
    struct http_header **headers;
};

/* SAX hash function */
static unsigned int http_hdr_table_offset(const char *header, size_t hl)
{
    unsigned int h = 0;
    int i;

    if (hl > 0)
        for (i = 0; i < hl; i++)
            h ^= ( h << 5 ) + ( h >> 2 ) + tolower(header[i]);
    else
        for(; *header; header++)
            h ^= ( h << 5 ) + ( h >> 2 ) + tolower(*header);

    return h % HEADER_TABLE_SIZE;
}

static http_hdr_table *http_hdr_table_init(void)
{
    int i;
    http_hdr_table *t;

    t = xmalloc(sizeof(http_hdr_table));
    t->headers = xmalloc(HEADER_TABLE_SIZE*sizeof(struct http_header *));

    for (i = 0; i < HEADER_TABLE_SIZE; i++) 
        t->headers[i] = NULL;

    t->size = 0;
    return t;
}

static void http_hdr_table_destroy(http_hdr_table *t)
{
    struct http_header *h;
    int i;

    for (i = 0; i < HEADER_TABLE_SIZE; i++) {
        if (!t->headers[i]) 
            continue;
        for (h = t->headers[i]; h; h = h->next) {
            xfree(h->value);
            xfree(h->header);
        }
        xfree(t->headers[i]);
    }
    xfree(t->headers);
    xfree(t);
}

static struct http_header *http_hdr_table_new_hdr(const char *header, 
        size_t hl, const char *value, size_t vl)
{
    struct http_header *h;

    h = xmalloc(sizeof(*h));
    h->header = xstrndup(header, hl);
    h->value = xstrndup(value, vl);
    h->next = NULL;
    return h;
}

static void http_hdr_table_set(http_hdr_table *t, const char *header, 
        size_t hl, const char *value, size_t vl)
{
    unsigned int i;
    struct http_header *h;

    assert(t);
    assert(header);
    assert(value);

    i = http_hdr_table_offset(header, hl);
    if (t->headers[i]) {
        h = t->headers[i];
        while (h->next) h = h->next;
        h->next = http_hdr_table_new_hdr(header, hl, value, vl);
    }
    else {
        t->headers[i] = http_hdr_table_new_hdr(header, hl, value, vl);
    }
    t->size++;
}

static char *http_hdr_table_get(http_hdr_table *t, const char *header)
{
    int i;
    struct http_header *h;

    assert(t);
    assert(header);

    i = http_hdr_table_offset(header, 0);
    if (!t->headers[i])
        return NULL;
    if (!t->headers[i]->next)
        return t->headers[i]->value;

    for (h = t->headers[i]; h; h = h->next) {
        if (strcasecmp(header, h->value) == 0)
            return h->value;
    }
    return NULL;
}

static int http_tocrlf_length(const char *s)
{
    int i;

    for (i = 0; !is_crlf(s+i); i++) {
        if (!s[i])
            return 0;
    }

    return i;
}

static int http_tosp_length(int sep, const char *s)
{
    int i;

    for (i = 0; s[i] != sep; i++) {
        if (is_ctl_char(s[i]) || is_sep_char(s[i])) return 0;
    }

    return i;
}

static int parse_http_version(struct http_version *v, const char **s)
{
    const char *p = *s;

    if (strncmp(p, "HTTP/", 5) != 0) 
        return -1;
    p += 5;

    if (!is_digit(p) || p[1] != '.' || !is_digit(p+2))
        return -1;

    v->major = *p-'0';
    v->minor = p[2]-'0';
    *s = p+3;
    return 0;
}

static int http_method_length(const char *s)
{
    return http_tosp_length(' ', s);
}

static int http_uri_length(const char *s)
{
    int i;

    for (i = 0; s[i] != ' '; i++) {
        if (!s[i]) return 0;
    }

    return i;
}

int http_request_line_parse(struct http_request_line *rline, const char *s)
{
    int methodl;
    int uril;
    const char *urib;
    const char *methodb;
    const char *versionb;
    char *uris;

    assert(rline);
    assert(s);

    memset(rline, 0, sizeof(*rline));

    methodb = s;
    methodl = http_method_length(methodb);
    if (!methodl)
        return -1;

    urib = methodb + methodl + 1;
    uril = http_uri_length(urib);
    if (!uril)
        return -1;

    versionb = urib + uril + 1;
    if (parse_http_version(&rline->version, &versionb) == -1)
        return -1;

    if (!is_crlf(versionb))
        return -1;

    rline->method = xstrndup(methodb, methodl);
    uris = xstrndup(urib, uril);
    if (uri_parse(&(rline->uri), uris) != 0) {
        xfree(rline->method);
        xfree(uris);
        return -1;
    }
    xfree(uris);

    return 0;
}

void http_request_line_destroy(struct http_request_line *rline)
{
    assert(rline);

    xfree(rline->method);
    uri_destroy(&rline->uri);
    memset(rline, 0, sizeof(*rline));
}

static int http_reason_length(const char *s)
{
    return http_tocrlf_length(s);
}

int http_status_line_parse(struct http_status_line *sline, const char *s)
{
    const char *p;
    const char *b;
    char *tmp;
    int reasonl;

    assert(sline);
    assert(s);

    p = b = s;
    
    /* status-line = HTTP-version SP status-code SP reason-phrase CRLF */
    if (parse_http_version(&sline->version, &p) == -1 || *p++ != ' ') 
        return -1;

    /* status-code = 3DIGIT */
    if (!is_digit(p) || !is_digit(p+1) || !is_digit(p+2) || *(p+3) != ' ')
        return -1;

    tmp = xstrndup(p, 3);
    sline->code = atoi(tmp);
    xfree(tmp);
    p += 4;

    reasonl = http_reason_length(p);
    if (!reasonl)
        return -1;
    sline->reason = xstrndup(p, reasonl);
    return 0;
}

void http_status_line_destroy(struct http_status_line *sline)
{
    assert(sline);

    xfree(sline->reason);
    memset(sline, 0, sizeof(*sline));
}

http_hdr_table *http_headers_init(void)
{
    return http_hdr_table_init();
}

void http_headers_destroy(http_hdr_table *t)
{
    assert(t);

    http_hdr_table_destroy(t);
}

static int http_field_name_length(const char *s)
{
    return http_tosp_length(':', s);
}

static int http_value_length(const char *s)
{
    return http_tocrlf_length(s);
}

int http_header_parse(http_hdr_table *t, const char *s)
{
    const char *hname;
    const char *hvalue;
    int hl;
    int vl;

    assert(t);
    assert(s);

    hname = s;
    hl = http_field_name_length(s);
    if (!hl)
        return -1;

    s += hl+1;
    while (*s == ' ' || *s == '\t') {
        s++;
    }

    hvalue = s;
    vl = http_value_length(s);
    if (!vl)
        return -1;

    http_hdr_table_set(t, hname, hl, hvalue, vl);

    return 0;
}

char *http_header_get(http_hdr_table *t, const char *header)
{
    return http_hdr_table_get(t, header);
}
