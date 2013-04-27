#include <stdlib.h>
#include <assert.h>
#include <string.h>

void *xmalloc(size_t size)
{
    void *ptr;

    assert(size);

    ptr = malloc(size);
    if (!ptr) 
        abort();

    return ptr;
}

void xfree(void *ptr)
{
    assert(ptr);
    free(ptr);
}

char *xstrndup(const char *str, size_t n)
{
    char *ret;
    int i;

    assert(str);
    assert(n);

    ret = xmalloc(n+1);
    for (i = 0; *str && i < n; i++, str++)
        ret[i] = *str;
    ret[i] = 0;
    return ret;
}

char *xstrdup(const char *s)
{
    char *ptr;

    assert(s);

    ptr = strdup(s);
    if (!ptr)
        abort();

    return ptr;
}

