/* This needs rewrite */
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "uri.h"
#include "xmalloc.h"

static int uri_scheme_length(const char *s)
{
    
}

int uri_parse(struct uri *uri, const char *uri_s)
{
    const char *p;
    const char *b;
    char *port;
    int i;

    assert(uri);
    assert(uri_s);

    memset(uri, 0, sizeof(*uri));

    /* RFC3986 3.  URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ] */
    /* RFC3986 3.1. scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
    for (p = b = uri_s; ; p++) {
        if (*p == ':') {
            if (p-b > 0) uri->scheme = xstrndup(uri_s, p-b);
            p++;
            break;
        }

        if (!*p) goto uri_error;
    }

    /*
     * The authority component is preceded by a double slash ("//") and is
     * terminated by the next slash ("/"), question mark ("?"), or number sign
     * ("#") character, or by the end of the URI.
     * */

    if (*p++ != '/' || *p++ != '/')
        goto uri_error;

    /* RFC3986 3.2 authority = [ userinfo "@" ] host [ ":" port ] */

    /* userinfo */
    for (b = p, i = 0; p[i]; i++) {
        if (p[i] == '@') {
            if (i) uri->authority.opt = xstrndup(b, i);
            p += i+1;
            b = p;
            break;
        }
    }

    /* ipv6 */
    if (*p == '[') {
        b = p+1;
        for (;; p++) {
            if (!*p) goto uri_error;
            if (*p == ']') break;
        }
    }

    /* host */
    for (; ; p++) {
        if (*p == '/' || *p == '?' || *p == '#' || *p == ']' || *p == ':' || !*p) {
            if (p-b > 0) uri->authority.hostname = xstrndup(b, p-b);
            else goto uri_error;
            if (!*p) return 0;
            if (*p == ':') ++p;
            if (*p == ']') { ++p; if (*p == ':') ++p; }
            break;
        }
    }

    /* port */
    if (*p >= '0' && *p <= '9') {
        for (b = p; ; p++) {
            if (*p == '/' || *p == '?' || *p == '#' || !*p) {
                if (p-b <= 0) break;
                port = xstrndup(b, p-b);
                uri->authority.port = atoi(port);
                xfree(port);
                if (!*p) return 0;
                break;
            }
        }
    }

    /* path  */
    for (b = p; ; p++) {
        if (*p == '?' || *p == '#' || !*p) {
            if (p-b > 0) uri->path = xstrndup(b, p-b);
            if (!*p) return 0;
            break;
        }
    }

    /* RFC3986 3.5 query = *( pchar / "/" / "?" ) */
    if (*p == '?') {
        for (b = ++p; ; p++) {
            if (*p == '#' || !*p) {
                if (p-b > 0) uri->query = xstrndup(b, p-b);
                if (!*p) return 0;
                break;
            }
        }
    }

    /* RFC3986 3.6 fragment = *( pchar / "/" / "?" ) */
    if (*p == '#') {
        for (b = ++p; ; p++) {
            if (!*p) {
                if (p-b > 0) uri->fragment = xstrndup(b, p-b);
                break;
            }
        }
    }

    return 0;

uri_error:
    uri_destroy(uri);
    return -1;
}

void uri_destroy(struct uri *uri)
{
    if (uri->scheme) xfree(uri->scheme);
    if (uri->authority.opt) xfree(uri->authority.opt);
    if (uri->authority.hostname) xfree(uri->authority.hostname);
    if (uri->path) xfree(uri->path);
    if (uri->query) xfree(uri->query);
    if (uri->fragment) xfree(uri->fragment);
    memset(uri, 0, sizeof(*uri));
}
