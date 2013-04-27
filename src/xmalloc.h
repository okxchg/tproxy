#ifndef XMALLOC_H
#define XMALLOC_H

void *xmalloc(size_t size);
void xfree(void *ptr);
char *xstrndup(const char *str, size_t n);
char *xstrdup(const char *s);

#endif /* XMALLOC_H */
