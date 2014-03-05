#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "uri.h"
#include "xmalloc.h"

/* We don't validate yet, we just cut the uri to pieces */

static int uri_parse_scheme(const char *schemeb, size_t *schemel, const char **hostb)
{
    size_t len;

    for (len = 0; schemeb[len] != ':'; len++) {
        /* no scheme */
        if (schemeb[len] == '\0')
            return -1;
    }

    /* no scheme */
    if (len == 0)
        return -1;

    *schemel = len;
    *hostb = schemeb + len + 1;
    return 0;
}

static int uria_parse_userinfo(const char *userinfob, size_t *userinfol, const char **hostb)
{
    size_t len;

    *hostb = userinfob;
    for (len = 0; userinfob[len] != '@'; len++) {
        /* no userinfo */
        if (userinfob[len] == '\0' || userinfob[len] == '/')
            return 0;
    }

    *hostb += len + 1;

    /* no userinfo */
    if (len == 0)
        return 0;

    *userinfol = len;
    return 1;
}

static int uria_parse_host_port(const char *base, const char **hostb,
        size_t *hostl, int *port, const char **newb)
{
    size_t i;
    const char *hostb__, *portb__, *newb__;
    size_t hostl__;
    int port__;
    

    /* IPv6, just check if [] are paired */
    i = 0;
    if (base[0] == '[') {
        for (;; i++) {
            if (base[i] == ']') {
                i++;
                break;
            }
            if (!base[i]) {
                return -1;
            }
        }
    }


    for (;; i++) {
        if (base[i] == '/' || base[i] == '?' || base[i] == '#' || 
            base[i] == ':' || base[i] == '\0') {
            if (i > 0) {
                hostb__ = base;
                hostl__ = i;
                portb__ = hostb__ + hostl__;
                newb__ = hostb__ + hostl__;
                break;
            }
            else {
                return -1;
            }
        }
    }

    if (hostb__[0] == '[') {
        if (hostb__[ hostl__-1 ] != ']' || hostl__ < 3) {
            return -1;
        }
        else {
            ++hostb__;
            hostl__ -= 2;
        }
    }

    port__ = 0;
    if (portb__[0] == ':') {
        for (i = 1; portb__[i] >= '0' && portb__[i] <= '9'; i++) {
            port__ = 10 * port__-'0' + portb__[i];
        }
        newb__ += i;
    }

    *port = port__;
    *hostb = hostb__;
    *hostl = hostl__;
    *newb = newb__;
    return 0;
}

static int uri_parse_path(const char *pathb, size_t *pathl, const char **queryb)
{
    size_t len;

    len = 0;
    while (pathb[len] != '?' && pathb[len] != '#' && pathb[len] != '\0') {
        len++;
    }

    *queryb = pathb + len;
    if (len) {
        *pathl = len;
        return 1;
    }
    else {
        return 0;
    }
}

static int uri_parse_query(const char **queryb, size_t *queryl, const char **fragb)
{
    size_t len;
    const char *queryb__;

    queryb__ = *queryb;
    if (queryb__[0] == '?') {
        queryb__++;
        len = 0;
        while (queryb__[len] != '#' && queryb__[len] != '\0') {
            len++;
        }
        *queryl = len;
        *fragb = queryb__ + len;
        *queryb = queryb__;
        return len;
    }
    else {
        *fragb = *queryb;
        return 0;
    }
}

static int uri_parse_fragment(const char **fragb, size_t *fragl)
{
    size_t len;
    const char *fragb__;

    fragb__ = *fragb;
    if (fragb__[0] == '#') {
        fragb__++;
        len = 0;
        while (fragb__[len] != '\0')
            len++;
        *fragl = len;
        *fragb = fragb__;
        return len;
    }
    else {
        return 0;
    }
}

int uri_parse(struct uri *uri, const char *urib)
{
    const char *schemeb;
    const char *authb;
    const char *userinfob;
    const char *hostb;
    const char *pathb;
    const char *queryb;
    const char *fragb;

    size_t schemel;
    size_t userinfol;
    size_t hostl;
    size_t queryl;
    size_t fragl;
    size_t pathl;
    int port;

    int haveuserinfo;
    int havepath;
    int havequery;
    int havefragment;
    
    assert(uri);
    assert(urib);

    memset(uri, 0, sizeof(*uri));

    /* First, get the lengths and base addresses of all elements */

    schemeb = urib;
    if (uri_parse_scheme(schemeb, &schemel, &authb) == -1)
        return -1;

    if (*authb++ != '/' || *authb++ != '/')
        return -1;

    userinfob = authb;
    haveuserinfo = uria_parse_userinfo(userinfob, &userinfol, &hostb);

    if (uria_parse_host_port(hostb, &hostb, &hostl, &port, &pathb) == -1)
        return -1;

    havepath = uri_parse_path(pathb, &pathl, &queryb);
    havequery = uri_parse_query(&queryb, &queryl, &fragb);
    havefragment = uri_parse_fragment(&fragb, &fragl);

    /* Now centralize the allocations so we don't have to have too much error
     * handling code */
    uri->scheme = xstrndup(schemeb, schemel);
    uri->authority.port = port;
    uri->authority.hostname = xstrndup(hostb, hostl);

    if (haveuserinfo)
        uri->authority.opt = xstrndup(userinfob, userinfol);

    if (havepath)
        uri->path = xstrndup(pathb, pathl);

    if (havequery)
        uri->query = xstrndup(queryb, queryl);

    if (havefragment)
        uri->fragment = xstrndup(fragb, fragl);

    return 0;
}

void uri_destroy(struct uri *uri)
{
    if (uri->scheme) 
        xfree(uri->scheme);
    if (uri->authority.opt)
        xfree(uri->authority.opt);
    if (uri->authority.hostname) 
        xfree(uri->authority.hostname);
    if (uri->path)
        xfree(uri->path);
    if (uri->query)
        xfree(uri->query);
    if (uri->fragment)
        xfree(uri->fragment);

    memset(uri, 0, sizeof(*uri));
}
